/*
    SPDX-FileCopyrightText: 2018 Nicolas Fella <nicolas.fella@gmx.de>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/
#pragma once
#include "maps.h"

namespace PulseAudioQt
{
class AbstractModelPrivate
{
public:
    explicit AbstractModelPrivate(AbstractModel *q, const MapBaseQObject *map);
    virtual ~AbstractModelPrivate();

    AbstractModel *q;
    const MapBaseQObject *m_map;
    QHash<int, QByteArray> m_roles;
    QHash<int, int> m_objectProperties;
    QHash<int, int> m_signalIndexToProperties;
};
}
