/*
 * SPDX-FileCopyrightText: 2009 Dario Freddi <drf@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <QApplication>
#include "KIdleTest.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    KIdleTest t;

    return app.exec();
}
