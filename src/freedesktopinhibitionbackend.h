/*  SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
    SPDX-FileCopyrightText: 2020 Nicolas Fella <nicolas.fella@gmx.de>
*/

#ifndef FREEDESKTOPINHIBITIONBACKEND
#define FREEDESKTOPINHIBITIONBACKEND

#include "abstractinhibitionbackend.h"

#include "screensaverdbusinterface.h"

class FreedesktopInhibitionBackend : public AbstractInhibitionBackend
{

public:
    explicit FreedesktopInhibitionBackend();

    void setInhibitionOff() override;
    void setInhibitionOn(QString reason = QString()) override;

private:
    OrgFreedesktopScreenSaverInterface m_iface;
    int m_cookie;
};

#endif
