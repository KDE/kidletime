/*
    SPDX-FileCopyrightText: 2021 David Edmundson <davidedmundson@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/
#include "poller.h"

#include <QDebug>
#include <QGuiApplication>
#include <QLoggingCategory>
#include <QWaylandClientExtensionTemplate>
#include <QtWaylandClient/QtWaylandClientVersion>

#include <qpa/qplatformnativeinterface.h>

#include "qwayland-idle.h"

Q_DECLARE_LOGGING_CATEGORY(POLLER)
Q_LOGGING_CATEGORY(POLLER, "kf5idletime_wayland")

class IdleTimeout : public QObject, public QtWayland::org_kde_kwin_idle_timeout
{
    Q_OBJECT
public:
    IdleTimeout(struct ::org_kde_kwin_idle_timeout* object)
        : QObject()
        , QtWayland::org_kde_kwin_idle_timeout(object)
    {}

    ~IdleTimeout() {
        release();
    }
Q_SIGNALS:
    void idle();
    void resumeFromIdle();
protected:
    void org_kde_kwin_idle_timeout_idle() override {
        Q_EMIT idle();
    }
    void org_kde_kwin_idle_timeout_resumed() override {
        Q_EMIT resumeFromIdle();
    }
};

class IdleManager : public QWaylandClientExtensionTemplate<IdleManager>, public QtWayland::org_kde_kwin_idle
{
public:
    IdleManager()
        : QWaylandClientExtensionTemplate<IdleManager>(1)
    {
#if QTWAYLANDCLIENT_VERSION >= QT_VERSION_CHECK(6, 2, 0)
        initialize();
#else
        // QWaylandClientExtensionTemplate invokes this with a QueuedConnection but we want shortcuts
        // to be inhibited immediately.
        QMetaObject::invokeMethod(this, "addRegistryListener");
#endif
    }
};

Poller::Poller(QObject *parent)
    : AbstractSystemPoller(parent)
    , m_idleManager(new IdleManager)
{

}

Poller::~Poller() = default;

bool Poller::isAvailable()
{
    return m_idleManager->isActive();
}

void Poller::addTimeout(int nextTimeout)
{
    if (m_timeouts.contains(nextTimeout)) {
        return;
    }

    auto timeout = createTimeout(nextTimeout);
    if (!timeout) {
        return;
    }

    connect(timeout, &IdleTimeout::idle, this, [this, nextTimeout] {
        Q_EMIT timeoutReached(nextTimeout);
    });
    connect(timeout, &IdleTimeout::resumeFromIdle, this, &Poller::resumingFromIdle);
    m_timeouts.insert(nextTimeout, QSharedPointer<IdleTimeout>(timeout));
}

void Poller::removeTimeout(int nextTimeout)
{
    m_timeouts.remove(nextTimeout);
}

QList<int> Poller::timeouts() const
{
    return QList<int>();
}

void Poller::catchIdleEvent()
{
    if (m_catchResumeTimeout) {
        // already setup
        return;
    }
    if (!isAvailable()) {
        return;
    }

    m_catchResumeTimeout.reset(createTimeout(0));
    if (!m_catchResumeTimeout) {
        return;
    }
    connect(m_catchResumeTimeout.get(), &IdleTimeout::resumeFromIdle, this, [this] {
        stopCatchingIdleEvents();
        Q_EMIT resumingFromIdle();
    });
}

void Poller::stopCatchingIdleEvents()
{
    m_catchResumeTimeout.reset();
}

int Poller::forcePollRequest()
{
    qCWarning(POLLER) << "This plugin does not support polling idle time";
    return 0;
}

void Poller::simulateUserActivity()
{
    // the timeout value doesn't matter as we're just calling one method on it then deleting
    QScopedPointer<IdleTimeout> timeout(createTimeout(UINT_MAX));
    if (timeout) {
        timeout->simulate_user_activity();
    }
}

IdleTimeout* Poller::createTimeout(int timeout)
{
    if (!isAvailable()) {
        return nullptr;
    }

    QPlatformNativeInterface *nativeInterface = qGuiApp->platformNativeInterface();
    if (!nativeInterface) {
        return nullptr;
    }
    auto seat = static_cast<wl_seat *>(nativeInterface->nativeResourceForIntegration("wl_seat"));
    if (!seat) {
        return nullptr;
    }

    return new IdleTimeout(m_idleManager->get_idle_timeout(seat, timeout));
}



#include "poller.moc"
