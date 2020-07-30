/*  SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
    SPDX-FileCopyrightText: 2020 Nicolas Fella <nicolas.fella@gmx.de>
*/

#include "androidinhibitionbackend.h"

#include <QDebug>
#include <QtAndroid>
#include <QAndroidJniObject>

void AndroidInhibitionBackend::setInhibitionOff()
{
    QtAndroid::runOnAndroidThread([] {
        int flag = QAndroidJniObject::getStaticField<int>("android.view.WindowManager.LayoutParams", "FLAG_KEEP_SCREEN_ON");
        QAndroidJniObject window = QtAndroid::androidActivity().callObjectMethod("getWindow", "()V");
        window.callMethod<void>("clearFlags", "(I)V", flag);
    });
}

void AndroidInhibitionBackend::setInhibitionOn(QString reason)
{
    Q_UNUSED(reason)

    QtAndroid::runOnAndroidThread([] {
        int flag = QAndroidJniObject::getStaticField<int>("android.view.WindowManager.LayoutParams", "FLAG_KEEP_SCREEN_ON");
        QAndroidJniObject window = QtAndroid::androidActivity().callObjectMethod("getWindow", "()V");
        window.callMethod<void>("addFlags", "(I)V", flag);
    });
}
