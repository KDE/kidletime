/*  SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
    SPDX-FileCopyrightText: 2020 Nicolas Fella <nicolas.fella@gmx.de>
*/

#include "kidleinhibition.h"

#include "abstractinhibitionbackend.h"
#include "stubinhibitionbackend.h"

#ifdef Q_OS_ANDROID
#include "androidinhibitionbackend.h"
#else
#include "freedesktopinhibitionbackend.h"
#endif

class KIdleInhibitionPrivate {
public:
#ifdef Q_OS_ANDROID
    AndroidInhibitionBackend m_backend;
#elif defined(Q_OS_LINUX) || defined(Q_OS_FREEBSD)
    FreedesktopInhibitionBackend m_backend;
#else
    StubInhibitionBackend m_backend;
#endif
    bool m_inhibited = false;
};

KIdleInhibition::KIdleInhibition(QObject *parent)
    : QObject(parent)
    , d(new KIdleInhibitionPrivate)
{
}

KIdleInhibition::~KIdleInhibition() = default;

void KIdleInhibition::setInhibitionOn(QString reason)
{
    d->m_backend.setInhibitionOn(reason);
}

void KIdleInhibition::setInhibitionOff()
{
    d->m_backend.setInhibitionOff();
}

void KIdleInhibition::toggleInhibition(QString reason)
{
    if (d->m_inhibited) {
        d->m_backend.setInhibitionOff();
    } else {
        d->m_backend.setInhibitionOn(reason);
    }

    d->m_inhibited = !d->m_inhibited;
}
