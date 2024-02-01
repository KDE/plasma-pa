/*
    SPDX-FileCopyrightText: 2018 Nicolas Fella <nicolas.fella@gmx.de>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/
#ifndef STREAM_P_H
#define STREAM_P_H

#include "stream.h"
#include "volumeobject_p.h"

namespace PulseAudioQt
{
class StreamPrivate
{
public:
    explicit StreamPrivate(Stream *q);
    virtual ~StreamPrivate();

    Stream *q;

    quint32 m_deviceIndex = PA_INVALID_INDEX;
    quint32 m_clientIndex = PA_INVALID_INDEX;
    bool m_virtualStream = false;
    bool m_corked = false;
    bool m_hasVolume = false;

    template<typename PAInfo>
    void updateStream(const PAInfo *info)
    {
        q->VolumeObject::d->updateVolumeObject(info);

        if (m_hasVolume != info->has_volume) {
            m_hasVolume = info->has_volume;
            Q_EMIT q->hasVolumeChanged();
        }
        if (q->VolumeObject::d->m_volumeWritable != info->volume_writable) {
            q->VolumeObject::d->m_volumeWritable = info->volume_writable;
            Q_EMIT q->isVolumeWritableChanged();
        }
        if (m_clientIndex != info->client) {
            m_clientIndex = info->client;
            Q_EMIT q->clientChanged();
        }
        if (m_virtualStream != (info->client == PA_INVALID_INDEX)) {
            m_virtualStream = info->client == PA_INVALID_INDEX;
            Q_EMIT q->virtualStreamChanged();
        }
        if (m_corked != info->corked) {
            m_corked = info->corked;
            Q_EMIT q->corkedChanged();
        }
    }
};
}
#endif
