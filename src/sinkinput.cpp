/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "sinkinput.h"

#include "context.h"
#include "context_p.h"

#include "sink.h"
#include "stream_p.h"

namespace PulseAudioQt
{
SinkInput::SinkInput(QObject *parent)
    : Stream(parent)
{
}

void SinkInput::update(const pa_sink_input_info *info)
{
    Stream::d->updateStream(info);
    if (Stream::d->m_deviceIndex != info->sink) {
        Stream::d->m_deviceIndex = info->sink;
        Q_EMIT deviceIndexChanged();
    }
}

void SinkInput::setDeviceIndex(quint32 deviceIndex)
{
    Context::instance()->d->setGenericDeviceForStream(index(), deviceIndex, &pa_context_move_sink_input_by_index);
}

void SinkInput::setVolume(qint64 volume)
{
    Context::instance()->d->setGenericVolume(index(), -1, volume, VolumeObject::d->cvolume(), &pa_context_set_sink_input_volume);
}

void SinkInput::setMuted(bool muted)
{
    Context::instance()->d->setGenericMute(index(), muted, &pa_context_set_sink_input_mute);
}

void SinkInput::setChannelVolume(int channel, qint64 volume)
{
    Context::instance()->d->setGenericVolume(index(), channel, volume, VolumeObject::d->cvolume(), &pa_context_set_sink_input_volume);
}

void SinkInput::setChannelVolumes(const QList<qint64> &channelVolumes)
{
    Context::instance()->d->setGenericVolumes(index(), channelVolumes, VolumeObject::d->cvolume(), &pa_context_set_sink_input_volume);
}

} // PulseAudioQt
