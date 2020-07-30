/*  SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
    SPDX-FileCopyrightText: 2020 Nicolas Fella <nicolas.fella@gmx.de>
*/

#ifndef STUBINHIBITIONBACKEND
#define STUBINHIBITIONBACKEND

#include "abstractinhibitionbackend.h"

class StubInhibitionBackend : public AbstractInhibitionBackend
{

public:
    void setInhibitionOff() override;
    void setInhibitionOn(QString reason = QString()) override;
};

#endif
