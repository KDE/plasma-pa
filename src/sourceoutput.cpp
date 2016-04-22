/*
    Copyright 2014-2015 Harald Sitter <sitter@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "sourceoutput.h"

#include "context.h"

namespace QPulseAudio
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
        emit deviceIndexChanged();
    }
}

void SourceOutput::setDeviceIndex(quint32 deviceIndex)
{
    context()->setGenericDeviceForStream(index(), deviceIndex, &pa_context_move_source_output_by_index);
}

void SourceOutput::setVolume(qint64 volume)
{
    context()->setGenericVolume(index(), -1, volume, cvolume(), &pa_context_set_source_output_volume);
}

void SourceOutput::setMuted(bool muted)
{
    context()->setGenericMute(index(), muted, &pa_context_set_source_output_mute);
}

void SourceOutput::setChannelVolume(int channel, qint64 volume)
{
    context()->setGenericVolume(index(), channel, volume, cvolume(), &pa_context_set_source_output_volume);
}

} // QPulseAudio
