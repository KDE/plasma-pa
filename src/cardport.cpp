/*
    SPDX-FileCopyrightText: 2018 Nicolas Fella <nicolas.fella@gmx.de>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/
#include "cardport.h"
#include "port_p.h"

namespace PulseAudioQt
{
CardPort::CardPort(QObject *parent)
    : Port(parent)
{
}

CardPort::~CardPort()
{
}

void CardPort::update(const pa_card_port_info *info)
{
    Port::d->setInfo(info);
    PulseObject::d->updateProperties(info);
}

} // PulseAudioQt
