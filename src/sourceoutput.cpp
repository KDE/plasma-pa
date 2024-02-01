/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "sourceoutput.h"
#include "sourceoutput_p.h"

#include "context.h"
#include "context_p.h"
#include "stream_p.h"

namespace PulseAudioQt
{
SourceOutput::SourceOutput(QObject *parent)
    : Stream(parent)
    , d(new SourceOutputPrivate(this))
{
}

SourceOutput::~SourceOutput()
{
    delete d;
}

SourceOutputPrivate::SourceOutputPrivate(SourceOutput *q)
    : q(q)
{
}

void SourceOutputPrivate::update(const pa_source_output_info *info)
{
    q->Stream::d->updateStream(info);
    if (q->Stream::d->m_deviceIndex != info->source) {
        q->Stream::d->m_deviceIndex = info->source;
        Q_EMIT q->deviceIndexChanged();
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

void SourceOutput::setChannelVolumes(const QVector<qint64> &channelVolumes)
{
    Context::instance()->d->setGenericVolumes(index(), channelVolumes, VolumeObject::d->m_volume, &pa_context_set_source_output_volume);
}

} // PulseAudioQt
