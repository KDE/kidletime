/*  SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
    SPDX-FileCopyrightText: 2020 Nicolas Fella <nicolas.fella@gmx.de>
*/

#ifndef ABSTRACTINHIBITIONBACKEND_H
#define ABSTRACTINHIBITIONBACKEND_H

#include <QString>

class AbstractInhibitionBackend
{
public:
    virtual ~AbstractInhibitionBackend() = default;
    virtual void setInhibitionOn(QString reason) = 0;
    virtual void setInhibitionOff() = 0;
};

#endif
