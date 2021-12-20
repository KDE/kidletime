/* This file is part of the KDE libraries
 * SPDX-FileCopyrightText: 2009 Dario Freddi <drf at kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "xscreensaverbasedpoller.h"

#include <config-kidletime.h>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <private/qtx11extras_p.h>
#else
#include <QX11Info>
#endif

#include <X11/Xlib.h>
#include <X11/extensions/scrnsaver.h>

XScreensaverBasedPoller::XScreensaverBasedPoller(QObject *parent)
    : WidgetBasedPoller(parent)
    , m_screenSaverIface(nullptr)
{
}

XScreensaverBasedPoller::~XScreensaverBasedPoller()
{
}

bool XScreensaverBasedPoller::additionalSetUp()
{
    m_screenSaverIface = new OrgFreedesktopScreenSaverInterface(QLatin1String("org.freedesktop.ScreenSaver"),
                                                                QLatin1String("/ScreenSaver"),
                                                                QDBusConnection::sessionBus(),
                                                                this);

    connect(m_screenSaverIface, &OrgFreedesktopScreenSaverInterface::ActiveChanged, this, &XScreensaverBasedPoller::screensaverActivated);

    return true;
}

void XScreensaverBasedPoller::screensaverActivated(bool activated)
{
    // We care only if it has been disactivated

    if (!activated) {
        m_screenSaverIface->SimulateUserActivity();
        Q_EMIT resumingFromIdle();
    }
}

int XScreensaverBasedPoller::getIdleTime()
{
    XScreenSaverInfo *mitInfo = nullptr;
    mitInfo = XScreenSaverAllocInfo();
    XScreenSaverQueryInfo(QX11Info::display(), DefaultRootWindow(QX11Info::display()), mitInfo);
    int ret = mitInfo->idle;
    XFree(mitInfo);
    return ret;
}

void XScreensaverBasedPoller::simulateUserActivity()
{
    stopCatchingIdleEvents();
    XResetScreenSaver(QX11Info::display());
    XFlush(QX11Info::display());
}
