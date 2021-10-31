/* This file is part of the KDE libraries
 * SPDX-FileCopyrightText: 2009 Dario Freddi <drf at kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef XSCREENSAVERBASEDPOLLER_H
#define XSCREENSAVERBASEDPOLLER_H

#include "widgetbasedpoller.h"

#include "screensaver_interface.h"

class XScreensaverBasedPoller : public WidgetBasedPoller
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.kidletime.AbstractSystemPoller" FILE "xcb.json")
    Q_INTERFACES(AbstractSystemPoller)

public:
    explicit XScreensaverBasedPoller(QObject *parent = nullptr);
    ~XScreensaverBasedPoller() override;

public Q_SLOTS:
    void simulateUserActivity() override;

private:
    bool additionalSetUp() override;

private Q_SLOTS:
    void screensaverActivated(bool activated);
    int getIdleTime() override;

private:
    OrgFreedesktopScreenSaverInterface *m_screenSaverIface;
};

#endif /* XSCREENSAVERBASEDPOLLER_H_ */
