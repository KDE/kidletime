/*  SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
    SPDX-FileCopyrightText: 2020 Nicolas Fella <nicolas.fella@gmx.de>
*/

#include "stubinhibitionbackend.h"

#include <QDebug>

#include "logging.h"

void StubInhibitionBackend::setInhibitionOff()
{
    qWarning(KIDLETIME) << "Idle inhibition is not implemented for this platform";
}

void StubInhibitionBackend::setInhibitionOn(const QString reason)
{
    Q_UNUSED(reason);
    qWarning(KIDLETIME) << "Idle inhibition is not implemented for this platform";
}
