/* This file is part of the KDE libraries
 * SPDX-FileCopyrightText: 2009 Dario Freddi <drf at kde.org>
 * SPDX-FileCopyrightText: 2021 Harald Sitter <sitter@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef KIDLETIME_H
#define KIDLETIME_H

#include <QHash>
#include <QObject>
#include <kidletime_export.h>
#include <memory>

#if __has_include(<chrono>)
#include <chrono>
#endif

class KIdleTimePrivate;

/*!
 * \class KIdleTime
 * \inmodule KIdleTime
 *
 * \brief KIdleTime is a singleton reporting information on idle time.
 *
 * It is useful not
 * only for finding out about the current idle time of the PC, but also for getting
 * notified upon idle time events, such as custom timeouts, or user activity.
 *
 * \note All the intervals and times in this library are in milliseconds, unless
 *       specified otherwise.
 *
 * \since 4.4
 */
class KIDLETIME_EXPORT KIdleTime : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(KIdleTime)
    Q_DISABLE_COPY(KIdleTime)

public:
    /*!
     * Returns the singleton instance. Use this method to access KIdleTime.
     */
    static KIdleTime *instance();

    ~KIdleTime() override;

    /*!
     * Retrieves the idle time of the system, in milliseconds
     */
    int idleTime() const;

    /*!
     * Returns the list of timeout identifiers associated with their duration, in milliseconds,
     * the library is currently listening to.
     *
     * \sa addIdleTimeout()
     * \sa removeIdleTimeout()
     * \sa timeoutReached()
     */
    QHash<int, int> idleTimeouts() const;

    /*!
     * Attempts to simulate user activity. This implies that after calling this
     * method, the idle time of the system will become 0 and eventually resumingFromIdle()
     * will be triggered.
     *
     * \sa resumingFromIdle()
     */
    void simulateUserActivity();

public Q_SLOTS:
    /*!
     * Adds a new timeout to catch. When calling this method, after the system will be idle for
     * \a msec milliseconds, the signal timeoutReached() will be triggered. Please note that until you will
     * call removeIdleTimeout() or removeAllIdleTimeouts(), the signal will be triggered every
     * time the system will be idle for \a msec milliseconds. This function also returns an unique
     * token for the timeout just added to allow easier identification.
     *
     * \a msec the time, in milliseconds, after which the signal will be triggered.
     *
     * Returns an unique identifier for the timeout being added, that will be streamed by timeoutReached().
     *
     * \sa removeIdleTimeout()
     * \sa removeAllIdleTimeouts()
     * \sa timeoutReached()
     */
    int addIdleTimeout(int msec);

#if __has_include(<chrono>)
    /*!
     * Convenience overload supporting C++ chrono types. May also be used with chrono literals.
     * \since 5.83
     */
    int addIdleTimeout(std::chrono::milliseconds msec)
    {
        return addIdleTimeout(int(msec.count()));
    }
#endif

    /*!
     * Stops catching the idle timeout identified by the token \a identifier,
     * if it was registered earlier with addIdleTimeout.
     * Otherwise does nothing.
     *
     * \a identifier the token returned from addIdleTimeout of the timeout you want to stop listening to
     */
    void removeIdleTimeout(int identifier);

    /*!
     * Stops catching every set timeout (if any). This means that after calling this method, the signal
     * timeoutReached() won't be called again until you will add another timeout.
     *
     * \sa timeoutReached()
     * \sa addIdleTimeout()
     */
    void removeAllIdleTimeouts();

    /*!
     * Catches the next resume from idle event. This means that whenever user activity will be registered, or
     * simulateUserActivity() is called, the signal resumingFromIdle() will be triggered.
     *
     * Please note that this method will trigger the signal just for the very first resume event after the call:
     * this means you explicitly have to request to track every single resume event you are interested in.
     *
     * \note This behavior is due to the fact that a resume event happens whenever the user sends an input to the
     *       system. This would lead to a massive amount of signals being delivered when the PC is being used.
     *       Moreover, you are usually interested in catching just significant resume events, such as the ones after
     *       a significant period of inactivity. For tracking user input, you can use the more efficient methods provided
     *       by Qt. The purpose of this library is just monitoring the activity of the user.
     *
     * \sa resumingFromIdle()
     * \sa simulateUserActivity()
     *
     */
    void catchNextResumeEvent();

    /*!
     * Stops listening for resume event. This function serves for canceling catchNextResumeEvent(), as it
     * will have effect just when catchNextResumeEvent() has been called and resumingFromIdle() not
     * yet triggered.
     *
     * \sa resumingFromIdle()
     * \sa catchNextResumeEvent()
     *
     */
    void stopCatchingResumeEvent();

Q_SIGNALS:
    /*!
     * Triggered, if KIdleTime is catching resume events, when the system resumes from an idle state. This means
     * that either simulateUserActivity() was called or the user sent an input to the system.
     *
     * \sa catchNextResumeEvent()
     */
    void resumingFromIdle();

    /*!
     * Triggered when the system has been idle for x milliseconds, identified by the previously set
     * timeout.
     *
     * This signal is triggered whenever each timeout previously registered with addIdleTimeout(int)
     * is reached. It is guaranteed that \a msec will exactly correspond to the identified timeout.
     *
     * \a identifier the identifier of the timeout the system has reached.
     *
     * \a msec the time, in milliseconds, the system has been idle for.
     *
     * \sa addIdleTimeout()
     * \sa removeIdleTimeout()
     */
    void timeoutReached(int identifier, int msec); // clazy:exclude=overloaded-signal

private:
    KIDLETIME_NO_EXPORT KIdleTime();

    std::unique_ptr<KIdleTimePrivate> const d_ptr;
};

#endif /* KIDLETIME_H */
