/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "sourceoutput.h"

#include "context.h"
#include "context_p.h"

namespace PulseAudioQt
{
SourceOutput::SourceOutput(QObject *parent)
    : Stream(parent)
{
}

void SourceOutput::update(const pa_source_output_info *info)
{
    updateStream(info);
    if (m_deviceIndex != info->source) {
        m_deviceIndex = info->source;
        Q_EMIT deviceIndexChanged();
    }
}

void SourceOutput::setDeviceIndex(quint32 deviceIndex)
{
    context()->d->setGenericDeviceForStream(index(), deviceIndex, &pa_context_move_source_output_by_index);
}

void SourceOutput::setVolume(qint64 volume)
{
    context()->d->setGenericVolume(index(), -1, volume, cvolume(), &pa_context_set_source_output_volume);
}

void SourceOutput::setMuted(bool muted)
{
    context()->d->setGenericMute(index(), muted, &pa_context_set_source_output_mute);
}

void SourceOutput::setChannelVolume(int channel, qint64 volume)
{
    context()->d->setGenericVolume(index(), channel, volume, cvolume(), &pa_context_set_source_output_volume);
}

void SourceOutput::setChannelVolumes(const QList<qint64> &channelVolumes)
{
    context()->d->setGenericVolumes(index(), channelVolumes, cvolume(), &pa_context_set_source_output_volume);
}

} // PulseAudioQt
