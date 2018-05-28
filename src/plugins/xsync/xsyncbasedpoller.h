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

#ifndef XSYNCBASEDPOLLER_H
#define XSYNCBASEDPOLLER_H

#include "abstractsystempoller.h"

#include <QDebug>

#include <config-kidletime.h>

#include <X11/Xlib.h>
#include <X11/extensions/sync.h>
#include <xcb/xcb.h>
#include "fixx11h.h"

class XSyncBasedPoller : public AbstractSystemPoller
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.kidletime.AbstractSystemPoller" FILE "xcb.json")
    Q_INTERFACES(AbstractSystemPoller)

public:
    static XSyncBasedPoller *instance();

    explicit XSyncBasedPoller(QObject *parent = nullptr);
    ~XSyncBasedPoller() override;

    bool isAvailable() override;
    bool setUpPoller() override;
    void unloadPoller() override;

    bool xcbEvent(xcb_generic_event_t *event);

public Q_SLOTS:
    void addTimeout(int nextTimeout) override;
    void removeTimeout(int nextTimeout) override;
    QList<int> timeouts() const override;
    int forcePollRequest() override;
    void catchIdleEvent() override;
    void stopCatchingIdleEvents() override;
    void simulateUserActivity() override;

private Q_SLOTS:
    int poll();
    void reloadAlarms();

private:
    void setAlarm(Display *dpy, XSyncAlarm *alarm, XSyncCounter counter,
                  XSyncTestType test, XSyncValue value);

private:
    Display *m_display;
    xcb_connection_t *m_xcb_connection;

    int                 m_sync_event;
    XSyncCounter        m_idleCounter;
    QHash<int, XSyncAlarm>   m_timeoutAlarm;
    XSyncAlarm          m_resetAlarm;
    bool m_available;
};

#endif /* XSYNCBASEDPOLLER_H */

