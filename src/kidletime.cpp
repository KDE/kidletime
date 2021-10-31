/* This file is part of the KDE libraries
 * SPDX-FileCopyrightText: 2009 Dario Freddi <drf at kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "kidletime.h"

#include <config-kidletime.h>

#include "abstractsystempoller.h"
#include "logging.h"

#include <QDir>
#include <QGuiApplication>
#include <QJsonArray>
#include <QPluginLoader>
#include <QPointer>
#include <QSet>

class KIdleTimeHelper
{
public:
    KIdleTimeHelper()
        : q(nullptr)
    {
    }
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
    KIdleTimePrivate()
        : catchResume(false)
        , currentId(0)
    {
    }

    void loadSystem();
    void unloadCurrentSystem();
    void resumingFromIdle();
    void timeoutReached(int msec);

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

    connect(d->poller.data(), &AbstractSystemPoller::resumingFromIdle, this, [d]() {
        d->resumingFromIdle();
    });
    connect(d->poller.data(), &AbstractSystemPoller::timeoutReached, this, [d](int msec) {
        d->timeoutReached(msec);
    });
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

    if (!d->associations.contains(identifier) || !d->poller) {
        return;
    }

    int msec = d->associations[identifier];

    d->associations.remove(identifier);

    if (!d->associations.values().contains(msec)) {
        d->poller.data()->removeTimeout(msec);
    }
}

void KIdleTime::removeAllIdleTimeouts()
{
    Q_D(KIdleTime);

    QHash<int, int>::iterator i = d->associations.begin();
    QSet<int> removed;
    removed.reserve(d->associations.size());

    while (i != d->associations.end()) {
        int msec = d->associations[i.key()];

        i = d->associations.erase(i);

        if (!removed.contains(msec) && d->poller) {
            d->poller.data()->removeTimeout(msec);
            removed.insert(msec);
        }
    }
}

static QStringList pluginCandidates()
{
    QStringList ret;
    const QStringList libPath = QCoreApplication::libraryPaths();
    for (const QString &path : libPath) {
        const QDir pluginDir(path + QLatin1String("/kf5/org.kde.kidletime.platforms"));
        if (!pluginDir.exists()) {
            continue;
        }
        for (const QString &entry : pluginDir.entryList(QDir::Files | QDir::NoDotAndDotDot)) {
            ret << pluginDir.absoluteFilePath(entry);
        }
    }
    return ret;
}

static bool checkPlatform(const QJsonObject &metadata, const QString &platformName)
{
    const QJsonArray platforms = metadata.value(QStringLiteral("MetaData")).toObject().value(QStringLiteral("platforms")).toArray();
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
            AbstractSystemPoller *poller = qobject_cast<AbstractSystemPoller *>(loader.instance());
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

void KIdleTimePrivate::resumingFromIdle()
{
    Q_Q(KIdleTime);

    if (catchResume) {
        Q_EMIT q->resumingFromIdle();
        q->stopCatchingResumeEvent();
    }
}

void KIdleTimePrivate::timeoutReached(int msec)
{
    Q_Q(KIdleTime);

    if (associations.values().contains(msec)) {
        const auto listKeys = associations.keys(msec);
        for (int key : listKeys) {
#if KIDLETIME_BUILD_DEPRECATED_SINCE(5, 76)
            Q_EMIT q->timeoutReached(key);
#endif
            Q_EMIT q->timeoutReached(key, msec);
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
