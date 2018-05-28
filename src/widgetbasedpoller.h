/* This file is part of the KDE libraries
   Copyright (C) 2009 Dario Freddi <drf at kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef WIDGETBASEDPOLLER_H
#define WIDGETBASEDPOLLER_H

#include <kidletime_export.h>

#include "abstractsystempoller.h"

class QTimer;
class QEvent;

class KIDLETIME_EXPORT WidgetBasedPoller : public AbstractSystemPoller
{
    Q_OBJECT

public:
    explicit WidgetBasedPoller(QObject *parent = nullptr);
    ~WidgetBasedPoller() override;

    bool isAvailable() override;
    bool setUpPoller() override;
    void unloadPoller() override;

protected:
    bool eventFilter(QObject *object, QEvent *event) override;

public Q_SLOTS:
    void addTimeout(int nextTimeout) override;
    void removeTimeout(int nextTimeout) override;
    QList<int> timeouts() const override;
    int forcePollRequest() override;
    void catchIdleEvent() override;
    void stopCatchingIdleEvents() override;

private Q_SLOTS:
    int poll();
    virtual int getIdleTime() = 0;
    void detectedActivity();
    void waitForActivity();
    void releaseInputLock();

private:
    virtual bool additionalSetUp() = 0;

private:
    QTimer *m_pollTimer;
    QWidget *m_grabber;
    QList<int> m_timeouts;
};

#endif /* WIDGETBASEDPOLLER_H */
