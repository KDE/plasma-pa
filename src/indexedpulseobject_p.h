/*
    SPDX-FileCopyrightText: 2021 Nicolas Fella <nicolas.fella@gmx.de>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/
#ifndef INDEXEDPULSEOBJECT_P_H
#define INDEXEDPULSEOBJECT_P_H

#include "debug.h"

#include "pulseobject_p.h"

#include <pulse/introspect.h>

namespace PulseAudioQt
{
class IndexedPulseObjectPrivate
{
public:
    explicit IndexedPulseObjectPrivate(IndexedPulseObject *q);
    virtual ~IndexedPulseObjectPrivate();

    PulseObject *q;
    quint32 m_index = 0;

    template<typename PAInfo>
    void updatePulseObject(PAInfo *info)
    {
        m_index = info->index;

        q->PulseObject::d->updatePulseObject(info);
    }
};
}
#endif
