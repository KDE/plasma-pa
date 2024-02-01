/*
    SPDX-FileCopyrightText: 2018 Nicolas Fella <nicolas.fella@gmx.de>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/
#ifndef CARD_P_H
#define CARD_P_H

#include "card.h"
#include "cardport.h"
#include "profile.h"
#include <QHash>
#include <QVector>
#include <pulse/introspect.h>

namespace PulseAudioQt
{
class CardPrivate
{
public:
    explicit CardPrivate(Card *q);
    virtual ~CardPrivate();

    Card *q;

    void update(const pa_card_info *info);

    QList<Profile *> m_profiles;
    quint32 m_activeProfileIndex;
    QList<CardPort *> m_ports;
};
}

#endif
