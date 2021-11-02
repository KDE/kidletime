/* This file is part of the KDE libraries
 * SPDX-FileCopyrightText: 2009 Dario Freddi <drf at kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef ABSTRACTSYSTEMPOLLER_H
#define ABSTRACTSYSTEMPOLLER_H

#include <kidletime_export.h>

#include <QObject>

#define AbstractSystemPoller_iid "org.kde.kidletime.AbstractSystemPoller"

class KIDLETIME_EXPORT AbstractSystemPoller : public QObject
{
    Q_OBJECT

public:
    AbstractSystemPoller(QObject *parent = nullptr);
    ~AbstractSystemPoller() override;

    virtual bool isAvailable() = 0;
    virtual bool setUpPoller() = 0;
    virtual void unloadPoller() = 0;

public Q_SLOTS:
    virtual void addTimeout(int nextTimeout) = 0;
    virtual void removeTimeout(int nextTimeout) = 0;
    virtual QList<int> timeouts() const = 0;
    virtual int forcePollRequest() = 0;
    virtual void catchIdleEvent() = 0;
    virtual void stopCatchingIdleEvents() = 0;
    virtual void simulateUserActivity() = 0;

Q_SIGNALS:
    void resumingFromIdle();
    void timeoutReached(int msec);
};

Q_DECLARE_INTERFACE(AbstractSystemPoller, AbstractSystemPoller_iid)

#endif /* ABSTRACTSYSTEMPOLLER_H */
