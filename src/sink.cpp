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

#include "sink.h"

#include "context.h"
#include "server.h"

namespace QPulseAudio
{

Sink::Sink(QObject *parent)
    : Device(parent)
{
    connect(context()->server(), &Server::defaultSinkChanged, this, &Sink::defaultChanged);
}

void Sink::update(const pa_sink_info *info)
{
    updateDevice(info);
}

void Sink::setVolume(qint64 volume)
{
    context()->setGenericVolume(index(), -1, volume, cvolume(), &pa_context_set_sink_volume_by_index);
}

void Sink::setMuted(bool muted)
{
    context()->setGenericMute(m_index, muted, &pa_context_set_sink_mute_by_index);
}

void Sink::setActivePortIndex(quint32 port_index)
{
    Port *port = qobject_cast<Port *>(ports().at(port_index));
    if (!port) {
        qCWarning(PLASMAPA) << "invalid port set request" << port_index;
        return;
    }
    context()->setGenericPort(index(), port->name(), &pa_context_set_sink_port_by_index);
}

void Sink::setChannelVolume(int channel, qint64 volume)
{
    context()->setGenericVolume(index(), channel, volume, cvolume(), &pa_context_set_sink_volume_by_index);
}

bool Sink::isDefault() const
{
    return context()->server()->defaultSink() == this;
}

void Sink::setDefault(bool enable)
{
    if (enable) {
        context()->server()->setDefaultSink(this);
    }
}

} // QPulseAudio
