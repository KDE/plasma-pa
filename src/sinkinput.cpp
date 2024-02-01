/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "sinkinput.h"
#include "sinkinput_p.h"

#include "context.h"
#include "context_p.h"
#include "stream_p.h"

namespace PulseAudioQt
{
SinkInput::SinkInput(QObject *parent)
    : Stream(parent)
    , d(new SinkInputPrivate(this))
{
}

SinkInputPrivate::SinkInputPrivate(SinkInput *q)
    : q(q)
{
}

SinkInput::~SinkInput()
{
    delete d;
}

void SinkInputPrivate::update(const pa_sink_input_info *info)
{
    q->Stream::d->updateStream(info);
    if (q->Stream::d->m_deviceIndex != info->sink) {
        q->Stream::d->m_deviceIndex = info->sink;
        Q_EMIT q->deviceIndexChanged();
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

void SinkInput::setChannelVolumes(const QVector<qint64> &channelVolumes)
{
    Context::instance()->d->setGenericVolumes(index(), channelVolumes, VolumeObject::d->m_volume, &pa_context_set_sink_input_volume);
}

} // PulseAudioQt
