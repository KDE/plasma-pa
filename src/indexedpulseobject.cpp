/*
    SPDX-FileCopyrightText: 2021 Nicolas Fella <nicolas.fella@gmx.de>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "indexedpulseobject.h"
#include "indexedpulseobject_p.h"

#include "context.h"

namespace PulseAudioQt
{
IndexedPulseObject::IndexedPulseObject(QObject *parent)
    : PulseObject(parent)
    , d(new IndexedPulseObjectPrivate(this))
{
}

IndexedPulseObject::~IndexedPulseObject()
{
    delete d;
}

IndexedPulseObjectPrivate::IndexedPulseObjectPrivate(IndexedPulseObject *q)
    : q(q)
{
}

IndexedPulseObjectPrivate::~IndexedPulseObjectPrivate()
{
}

quint32 IndexedPulseObject::index() const
{
    return d->m_index;
}

} // PulseAudioQt
