/*  SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
    SPDX-FileCopyrightText: 2020 Nicolas Fella <nicolas.fella@gmx.de>
*/

#ifndef ANDROIDINHIBITIONBACKEND
#define ANDROIDINHIBITIONBACKEND

#include <QObject>
#include "abstractinhibitionbackend.h"

class AndroidInhibitionBackend : public AbstractInhibitionBackend
{

public:
    void setInhibitionOff() override;
    void setInhibitionOn(QString reason) override;
};

#endif
