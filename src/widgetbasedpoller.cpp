/* This file is part of the KDE libraries
 * SPDX-FileCopyrightText: 2009 Dario Freddi <drf at kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "widgetbasedpoller.h"

#include <QEvent>
#include <QTimer>
#include <QWindow>

WidgetBasedPoller::WidgetBasedPoller(QObject *parent)
    : AbstractSystemPoller(parent)
{
}

WidgetBasedPoller::~WidgetBasedPoller()
{
}

bool WidgetBasedPoller::isAvailable()
{
    return true;
}

bool WidgetBasedPoller::setUpPoller()
{
    m_pollTimer = new QTimer(this);

    // setup idle timer, with some smart polling
    connect(m_pollTimer, &QTimer::timeout, this, &WidgetBasedPoller::poll);

    m_grabber = new QWindow();
    m_grabber->setFlag(Qt::X11BypassWindowManagerHint);
    m_grabber->setPosition(-1000, -1000);
    m_grabber->installEventFilter(this);
    m_grabber->setObjectName(QStringLiteral("KIdleGrabberWidget"));

    return additionalSetUp();
}

void WidgetBasedPoller::unloadPoller()
{
    m_pollTimer->deleteLater();
    m_grabber->deleteLater();
}

QList<int> WidgetBasedPoller::timeouts() const
{
    return m_timeouts;
}

void WidgetBasedPoller::addTimeout(int nextTimeout)
{
    m_timeouts.append(nextTimeout);
    poll();
}

bool WidgetBasedPoller::eventFilter(QObject *object, QEvent *event)
{
    if (object == m_grabber && (event->type() == QEvent::MouseMove || event->type() == QEvent::KeyPress)) {
        detectedActivity();
        return true;
    } else if (object != m_grabber) {
        // If it's not the grabber, fallback to default event filter
        return false;
    }

    // Otherwise, simply ignore it
    return false;
}

void WidgetBasedPoller::waitForActivity()
{
    m_grabber->show();
    m_grabber->setMouseGrabEnabled(true);
    m_grabber->setKeyboardGrabEnabled(true);
}

void WidgetBasedPoller::detectedActivity()
{
    stopCatchingIdleEvents();
    Q_EMIT resumingFromIdle();
}

void WidgetBasedPoller::releaseInputLock()
{
    m_grabber->setMouseGrabEnabled(false);
    m_grabber->setKeyboardGrabEnabled(false);
    m_grabber->hide();
}

int WidgetBasedPoller::poll()
{
    int idle = getIdleTime();

    // Check if we reached a timeout..
    for (int timeOut : std::as_const(m_timeouts)) {
        if ((timeOut - idle < 300 && timeOut >= idle) || (idle - timeOut < 300 && idle > timeOut)) {
            // Bingo!
            Q_EMIT timeoutReached(timeOut);
        }
    }

    // Let's check the timer now!
    int mintime = 0;

    for (int i : std::as_const(m_timeouts)) {
        if (i > idle && (i < mintime || mintime == 0)) {
            mintime = i;
        }
    }

    // qDebug() << "mintime " << mintime << "idle " << idle;

    if (mintime != 0) {
        m_pollTimer->start(mintime - idle);
    } else {
        m_pollTimer->stop();
    }

    return idle;
}

int WidgetBasedPoller::forcePollRequest()
{
    return poll();
}

void WidgetBasedPoller::removeTimeout(int timeout)
{
    m_timeouts.removeOne(timeout);
    poll();
}

void WidgetBasedPoller::catchIdleEvent()
{
    waitForActivity();
}

void WidgetBasedPoller::stopCatchingIdleEvents()
{
    releaseInputLock();
}
