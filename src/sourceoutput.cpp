/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "sourceoutput.h"

#include "context.h"
#include "context_p.h"
#include "volumeobject_p.h"
#include "stream_p.h"

namespace PulseAudioQt
{
SourceOutput::SourceOutput(QObject *parent)
    : Stream(parent)
{
}

void SourceOutput::update(const pa_source_output_info *info)
{
    Stream::d->updateStream(info);
    if (Stream::d->m_deviceIndex != info->source) {
        Stream::d->m_deviceIndex = info->source;
        Q_EMIT deviceIndexChanged();
    }
}

void SourceOutput::setDeviceIndex(quint32 deviceIndex)
{
    Context::instance()->d->setGenericDeviceForStream(index(), deviceIndex, &pa_context_move_source_output_by_index);
}

void SourceOutput::setVolume(qint64 volume)
{
    Context::instance()->d->setGenericVolume(index(), -1, volume, VolumeObject::d->cvolume(), &pa_context_set_source_output_volume);
}

void SourceOutput::setMuted(bool muted)
{
    Context::instance()->d->setGenericMute(index(), muted, &pa_context_set_source_output_mute);
}

void SourceOutput::setChannelVolume(int channel, qint64 volume)
{
    Context::instance()->d->setGenericVolume(index(), channel, volume, VolumeObject::d->cvolume(), &pa_context_set_source_output_volume);
}

void SourceOutput::setChannelVolumes(const QList<qint64> &channelVolumes)
{
    Context::instance()->d->setGenericVolumes(index(), channelVolumes, VolumeObject::d->cvolume(), &pa_context_set_source_output_volume);
}

} // PulseAudioQt
