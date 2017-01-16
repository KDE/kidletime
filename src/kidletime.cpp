/* This file is part of the KDE libraries
   Copyright (C) 2009 Dario Freddi <drf at kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
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
    delete d_ptr;
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

    QHash< int, int >::iterator i = d->associations.begin();
    QSet< int > removed;
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
    foreach (const QString &path, QCoreApplication::libraryPaths()) {
        QDir pluginDir(path + QLatin1String("/kf5/org.kde.kidletime.platforms"));
        if (!pluginDir.exists()) {
            continue;
        }
        foreach (const QString &entry, pluginDir.entryList(QDir::Files | QDir::NoDotAndDotDot)) {
            ret << pluginDir.absoluteFilePath(entry);
        }
    }
    return ret;
}

static AbstractSystemPoller *loadPoller()
{
    foreach (const QString &candidate, pluginCandidates()) {
        if (!QLibrary::isLibrary(candidate)) {
            continue;
        }
        QPluginLoader loader(candidate);
        QJsonObject metaData = loader.metaData();
        const QJsonArray platforms = metaData.value(QStringLiteral("MetaData")).toObject().value(QStringLiteral("platforms")).toArray();
        for (auto it = platforms.begin(); it != platforms.end(); ++it) {
            if (QString::compare(QGuiApplication::platformName(), (*it).toString(), Qt::CaseInsensitive) == 0) {
                AbstractSystemPoller *poller = qobject_cast< AbstractSystemPoller* >(loader.instance());
                if (poller) {
                    qCDebug(KIDLETIME) << "Trying plugin" << candidate;
                    if (poller->isAvailable()) {
                        qCDebug(KIDLETIME) << "Using" << candidate << "for platform" << QGuiApplication::platformName();
                        return poller;
                    }
                    delete poller;
                }
            }
        }
    }
    return Q_NULLPTR;
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
        emit q->resumingFromIdle();
        q->stopCatchingResumeEvent();
    }
}

void KIdleTimePrivate::_k_timeoutReached(int msec)
{
    Q_Q(KIdleTime);

    if (associations.values().contains(msec)) {
        Q_FOREACH (int key, associations.keys(msec)) {
            emit q->timeoutReached(key);
            emit q->timeoutReached(key, msec);
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
