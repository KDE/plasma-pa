/*
    SPDX-FileCopyrightText: 2018 Nicolas Fella <nicolas.fella@gmx.de>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/
#ifndef CARDPORT_H
#define CARDPORT_H

#include "port.h"

#include <QObject>
#include <pulse/introspect.h>

namespace PulseAudioQt
{
/**
 * A Port associated with a Card.
 */
class PULSEAUDIOQT_EXPORT CardPort : public Port
{
    Q_OBJECT

public:
    ~CardPort();

    void update(const pa_card_port_info *info);

private:
    explicit CardPort(QObject *parent);

    friend class CardPrivate;
};

} // PulseAudioQt

#endif
