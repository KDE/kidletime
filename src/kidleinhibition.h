/*  SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
    SPDX-FileCopyrightText: 2020 Nicolas Fella <nicolas.fella@gmx.de>
*/

#ifndef KIDLEINHIBITION
#define KIDLEINHIBITION

#include <QObject>

#include <kidletime_export.h>

#include <memory>

class KIdleInhibitionPrivate;

/**
 * @class KIdleInhibition kidleinhibition.h KIdleInhibition
 *
 * Allows inhibiting idle actions such as automatic screen locking.
 *
 * Currenty freedesktop.org platforms (such as Linux and *BSD) and Android are supported.
 *
 * @since 5.74
 */
class KIDLETIME_EXPORT KIdleInhibition : public QObject
{
    Q_OBJECT
public:
    /**
     * Constructs a new inhibition object.
     * No actions are inhibited until setInhibitionOn is called.
     */
    explicit KIdleInhibition(QObject *parent=nullptr);
    virtual ~KIdleInhibition();

    /**
     * Turn this inhibition on.
     * @param reason A human-readable and translated description
     * of why the inhibition is in place.
     */
    Q_INVOKABLE void setInhibitionOn(QString reason = QString());

    /**
     * Turn this inhibition off.
     */
    Q_INVOKABLE void setInhibitionOff();

    /**
     * Toggle this inhibition.
     *
     * @param reason A human-readable and translated description
     * of why the inhibition is in place.
     */
    Q_INVOKABLE void toggleInhibition(QString reason = QString());

private:
    std::unique_ptr<KIdleInhibitionPrivate> d;
};

#endif
