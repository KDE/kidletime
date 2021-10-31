/* This file is part of the KDE libraries
 * SPDX-FileCopyrightText: 2009 Dario Freddi <drf at kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef WINDOWSPOLLER_H
#define WINDOWSPOLLER_H

#include "widgetbasedpoller.h"

class QTimer;

class WindowsPoller : public WidgetBasedPoller
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.kidletime.AbstractSystemPoller" FILE "windows.json")
    Q_INTERFACES(AbstractSystemPoller)

public:
    WindowsPoller(QObject *parent = 0);
    virtual ~WindowsPoller();

public Q_SLOTS:
    void simulateUserActivity();
    void catchIdleEvent();
    void stopCatchingIdleEvents();

private:
    bool additionalSetUp();

private Q_SLOTS:
    int getIdleTime();
    void checkForIdle();

private:
    QTimer *m_idleTimer;
};

#endif /* WINDOWSPOLLER_H */
