/*  SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
    SPDX-FileCopyrightText: 2020 Nicolas Fella <nicolas.fella@gmx.de>
*/

#include "freedesktopinhibitionbackend.h"

#include <QDBusConnection>
#include <QGuiApplication>

#include "logging.h"

FreedesktopInhibitionBackend::FreedesktopInhibitionBackend()
    : m_iface(QStringLiteral("org.freedesktop.ScreenSaver"), QStringLiteral("/org/freedesktop/ScreenSaver"), QDBusConnection::sessionBus())
    , m_cookie(0)
{
}

void FreedesktopInhibitionBackend::setInhibitionOff()
{
    m_iface.UnInhibit(m_cookie);
}

void FreedesktopInhibitionBackend::setInhibitionOn(const QString reason)
{
    const QString desktopEntry = QGuiApplication::desktopFileName();

    if (desktopEntry.isEmpty()) {
        qWarning(KIDLETIME) << "No desktop entry given. Please use QGuiApplication::setDesktopFileName() to set one";
    }

    m_cookie = m_iface.Inhibit(desktopEntry, reason);
}


