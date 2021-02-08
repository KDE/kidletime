/* This file is part of the KDE libraries
 * SPDX-FileCopyrightText: 2009 Dario Freddi <drf at kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-only
 */

#include "kidletime.h"

#include <config-kidletime.h>

#include "abstractsystempoller.h"
#include "logging.h"

#include <QDir>
#include <QJsonArray>
#include <QGuiApplication>
#include <QPluginLoader>
#include <QPointer>
#include <QSet>

class KIdleTimeHelper
{
public:
    KIdleTimeHelper() : q(nullptr) {}
    ~KIdleTimeHelper()
    {
        delete q;
    }
    KIdleTimeHelper(const KIdleTimeHelper &) = delete;
    KIdleTimeHelper &operator=(const KIdleTimeHelper &) = delete;
    KIdleTime *q;
};

Q_GLOBAL_STATIC(KIdleTimeHelper, s_globalKIdleTime)

KIdleTime *KIdleTime::instance()
{
    if (!s_globalKIdleTime()->q) {
        new KIdleTime;
    }

    return s_globalKIdleTime()->q;
}

class KIdleTimePrivate
{
    Q_DECLARE_PUBLIC(KIdleTime)
    KIdleTime *q_ptr;
public:
    KIdleTimePrivate() : catchResume(false), currentId(0) {}

    void loadSystem();
    void unloadCurrentSystem();
    void _k_resumingFromIdle();
    void _k_timeoutReached(int msec);

    QPointer<AbstractSystemPoller> poller;
    bool catchResume;

    int currentId;
    QHash<int, int> associations;
};

KIdleTime::KIdleTime()
    : QObject(nullptr)
    , d_ptr(new KIdleTimePrivate())
{
    Q_ASSERT(!s_globalKIdleTime()->q);
    s_globalKIdleTime()->q = this;

    d_ptr->q_ptr = this;

    Q_D(KIdleTime);
    d->loadSystem();

    connect(d->poller.data(), SIGNAL(resumingFromIdle()), this, SLOT(_k_resumingFromIdle()));
    connect(d->poller.data(), SIGNAL(timeoutReached(int)), this, SLOT(_k_timeoutReached(int)));
}

KIdleTime::~KIdleTime()
{
    Q_D(KIdleTime);
    d->unloadCurrentSystem();
}

void KIdleTime::catchNextResumeEvent()
{
    Q_D(KIdleTime);

    if (!d->catchResume && d->poller) {
        d->catchResume = true;
        d->poller.data()->catchIdleEvent();
    }
}

void KIdleTime::stopCatchingResumeEvent()
{
    Q_D(KIdleTime);

    if (d->catchResume && d->poller) {
        d->catchResume = false;
        d->poller.data()->stopCatchingIdleEvents();
    }
}

int KIdleTime::addIdleTimeout(int msec)
{
    Q_D(KIdleTime);
    if (Q_UNLIKELY(!d->poller)) {
        return 0;
    }

    d->poller.data()->addTimeout(msec);

    ++d->currentId;
    d->associations[d->currentId] = msec;

    return d->currentId;
}

void KIdleTime::removeIdleTimeout(int identifier)
{
    Q_D(KIdleTime);

    const auto it = d->associations.constFind(identifier);
    if (it == d->associations.cend() || !d->poller) {
        return;
    }

    const int msec = it.value();

    d->associations.erase(it);

    const bool isFound = std::any_of(d->associations.cbegin(), d->associations.cend(), [msec](int i) {
        return i == msec;
    });

    if (!isFound) {
        d->poller.data()->removeTimeout(msec);
    }
}

void KIdleTime::removeAllIdleTimeouts()
{
    Q_D(KIdleTime);

    QSet<int> removed;
    removed.reserve(d->associations.size());

    auto it = d->associations.begin();
    while (it != d->associations.end()) {
        const int msec = it.value();

        it = d->associations.erase(it);

        if (!removed.contains(msec) && d->poller) {
            d->poller.data()->removeTimeout(msec);
            removed.insert(msec);
        }
    }
}

static QStringList pluginCandidates()
{
    QStringList ret;
    const QStringList libPaths = QCoreApplication::libraryPaths();
    for (const QString &path : libPaths) {
        const QDir pluginDir(path + QLatin1String("/kf5/org.kde.kidletime.platforms"));
        if (!pluginDir.exists()) {
            continue;
        }

        const auto entries = pluginDir.entryList(QDir::Files | QDir::NoDotAndDotDot);
        for (const QString &entry : entries) {
            ret << pluginDir.absoluteFilePath(entry);
        }
    }

    return ret;
}

static bool checkPlatform(const QJsonObject &metadata, const QString &platformName)
{
    const QJsonArray platforms = metadata.value(QStringLiteral("MetaData"))
            .toObject().value(QStringLiteral("platforms")).toArray();
    return std::any_of(platforms.begin(), platforms.end(), [&platformName](const QJsonValue &value) {
        return QString::compare(platformName, value.toString(), Qt::CaseInsensitive) == 0;
    });
}

static AbstractSystemPoller *loadPoller()
{
    const QString platformName = QGuiApplication::platformName();

    const QVector<QStaticPlugin> staticPlugins = QPluginLoader::staticPlugins();
    for (const QStaticPlugin &staticPlugin : staticPlugins) {
        const QJsonObject metadata = staticPlugin.metaData();
        if (metadata.value(QLatin1String("IID")) != QLatin1String(AbstractSystemPoller_iid)) {
            continue;
        }
        if (checkPlatform(metadata, platformName)) {
            AbstractSystemPoller *poller = qobject_cast<AbstractSystemPoller *>(staticPlugin.instance());
            if (poller) {
                if (poller->isAvailable()) {
                    qCDebug(KIDLETIME) << "Loaded system poller from a static plugin";
                    return poller;
                }
                delete poller;
            }
        }
    }

    const QStringList lstPlugins = pluginCandidates();
    for (const QString &candidate : lstPlugins) {
        if (!QLibrary::isLibrary(candidate)) {
            continue;
        }
        QPluginLoader loader(candidate);
        if (checkPlatform(loader.metaData(), platformName)) {
            AbstractSystemPoller *poller = qobject_cast< AbstractSystemPoller* >(loader.instance());
            if (poller) {
                qCDebug(KIDLETIME) << "Trying plugin" << candidate;
                if (poller->isAvailable()) {
                    qCDebug(KIDLETIME) << "Using" << candidate << "for platform" << platformName;
                    return poller;
                }
                delete poller;
            }
        }
    }

    qCWarning(KIDLETIME) << "Could not find any system poller plugin";
    return nullptr;
}

void KIdleTimePrivate::loadSystem()
{
    if (!poller.isNull()) {
        unloadCurrentSystem();
    }

    // load plugin
    poller = loadPoller();

    if (poller && !poller->isAvailable()) {
        poller = nullptr;
    }
    if (!poller.isNull()) {
        poller.data()->setUpPoller();
    }
}

void KIdleTimePrivate::unloadCurrentSystem()
{
    if (!poller.isNull()) {
        poller.data()->unloadPoller();
        poller.data()->deleteLater();
    }
}

void KIdleTimePrivate::_k_resumingFromIdle()
{
    Q_Q(KIdleTime);

    if (catchResume) {
        Q_EMIT q->resumingFromIdle();
        q->stopCatchingResumeEvent();
    }
}

void KIdleTimePrivate::_k_timeoutReached(int msec)
{
    Q_Q(KIdleTime);

    for (auto it = associations.cbegin(); it != associations.cend(); ++it) {
        if (it.value() == msec) {
#if KIDLETIME_BUILD_DEPRECATED_SINCE(5, 76)
            Q_EMIT q->timeoutReached(it.key());
#endif
            Q_EMIT q->timeoutReached(it.key(), msec);
        }
    }
}

void KIdleTime::simulateUserActivity()
{
    Q_D(KIdleTime);

    if (Q_LIKELY(d->poller)) {
        d->poller.data()->simulateUserActivity();
    }
}

int KIdleTime::idleTime() const
{
    Q_D(const KIdleTime);
    if (Q_LIKELY(d->poller)) {
        return d->poller.data()->forcePollRequest();
    }
    return 0;
}

QHash<int, int> KIdleTime::idleTimeouts() const
{
    Q_D(const KIdleTime);

    return d->associations;
}

#include "moc_kidletime.cpp"
