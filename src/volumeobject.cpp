/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include <pulse/volume.h>

#include "volumeobject.h"
#include "volumeobject_p.h"

namespace PulseAudioQt
{
VolumeObject::VolumeObject(QObject *parent)
    : IndexedPulseObject(parent)
    , d(new VolumeObjectPrivate(this))
{
}

VolumeObjectPrivate::VolumeObjectPrivate(VolumeObject *q)
    : q(q)
{
    pa_cvolume_init(&m_volume);
}

VolumeObject::~VolumeObject()
{
    delete d;
}

qint64 VolumeObject::volume() const
{
    return pa_cvolume_max(&d->m_volume);
}

bool VolumeObject::isMuted() const
{
    return d->m_muted;
}

pa_cvolume VolumeObjectPrivate::cvolume() const
{
    return m_volume;
}

bool VolumeObject::isVolumeWritable() const
{
    return d->m_volumeWritable;
}

QVector<QString> VolumeObject::channels() const
{
    return d->m_channels;
}

QStringList VolumeObject::rawChannels() const
{
    return d->m_rawChannels;
}

QVector<qint64> VolumeObject::channelVolumes() const
{
    QVector<qint64> ret;
    ret.reserve(d->m_volume.channels);
    for (int i = 0; i < d->m_volume.channels; ++i) {
        ret << d->m_volume.values[i];
    }
    return ret;
}

} // PulseAudioQt
