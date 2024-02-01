/*
    SPDX-FileCopyrightText: 2018 Nicolas Fella <nicolas.fella@gmx.de>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/
#ifndef VOLUMEOBJECT_P_H
#define VOLUMEOBJECT_P_H

#include <pulse/volume.h>

#include "indexedpulseobject_p.h"
#include "volumeobject.h"

namespace PulseAudioQt
{
class VolumeObjectPrivate
{
public:
    explicit VolumeObjectPrivate(VolumeObject *q);

    VolumeObject *q;
    pa_channel_map foo;
    pa_cvolume m_volume;
    bool m_muted = true;
    bool m_volumeWritable = true;
    QVector<QString> m_channels;
    QStringList m_rawChannels;

    pa_cvolume cvolume() const;

    template<typename PAInfo>
    void updateVolumeObject(PAInfo *info)
    {
        q->IndexedPulseObject::d->updatePulseObject(info);
        q->PulseObject::d->updateProperties(info);
        if (m_muted != info->mute) {
            m_muted = info->mute;
            Q_EMIT q->mutedChanged();
        }
        if (!pa_cvolume_equal(&m_volume, &info->volume)) {
            m_volume = info->volume;
            Q_EMIT q->volumeChanged();
            Q_EMIT q->channelVolumesChanged();
        }
        QVector<QString> infoChannels;
        infoChannels.reserve(info->channel_map.channels);
        for (int i = 0; i < info->channel_map.channels; ++i) {
            infoChannels << QString::fromUtf8(pa_channel_position_to_pretty_string(info->channel_map.map[i]));
        }
        if (m_channels != infoChannels) {
            m_channels = infoChannels;
            Q_EMIT q->channelsChanged();
        }

        QStringList infoRawChannels;
        infoRawChannels.reserve(info->channel_map.channels);
        for (int i = 0; i < info->channel_map.channels; ++i) {
            infoRawChannels << QString::fromUtf8(pa_channel_position_to_string(info->channel_map.map[i]));
        }
        if (m_rawChannels != infoRawChannels) {
            m_rawChannels = infoRawChannels;
            Q_EMIT q->rawChannelsChanged();
        }
    }
};
}
#endif
