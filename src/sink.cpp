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
#include "canberracontext.h"
#include <pulse/channelmap.h>

namespace QPulseAudio
{

Sink::Sink(QObject *parent)
    : Device(parent)
{
    connect(context()->server(), &Server::defaultSinkChanged, this, &Sink::defaultChanged);
    CanberraContext::instance()->ref();
}

Sink::~Sink()
{
    CanberraContext::instance()->unref();
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
    if (!isDefault() && enable) {
        context()->server()->setDefaultSink(this);
    }
}

pa_channel_position_t Sink::channelNameToPosition(const QString &name)
{
    if (name == QLatin1String("Front Left")) {
        return PA_CHANNEL_POSITION_FRONT_LEFT;
    } else if (name == QLatin1String("Front Center")) {
        return PA_CHANNEL_POSITION_FRONT_CENTER;
    } else if (name == QLatin1String("Front Right")) {
        return PA_CHANNEL_POSITION_FRONT_RIGHT;
    } else if (name == QLatin1String("Side Left")) {
        return PA_CHANNEL_POSITION_SIDE_LEFT;
    } else if (name == QLatin1String("Side Right")) {
        return PA_CHANNEL_POSITION_SIDE_RIGHT;
    } else if (name == QLatin1String("Rear Left")) {
        return PA_CHANNEL_POSITION_REAR_LEFT;
    } else if (name == QLatin1String("Rear Right")) {
        return PA_CHANNEL_POSITION_REAR_RIGHT;
    } else if (name == QLatin1String("Subwoofer")) {
        return PA_CHANNEL_POSITION_SUBWOOFER;
    }

    return PA_CHANNEL_POSITION_MONO;
}

QString Sink::positionToChannelName(pa_channel_position_t position)
{
    switch (position) {
    case PA_CHANNEL_POSITION_FRONT_LEFT:
        return QStringLiteral("Front Left");
    case PA_CHANNEL_POSITION_FRONT_RIGHT:
        return QStringLiteral("Front Right");
    case PA_CHANNEL_POSITION_FRONT_CENTER:
        return QStringLiteral("Front Center");
    case PA_CHANNEL_POSITION_SIDE_LEFT:
        return QStringLiteral("Side Left");
    case PA_CHANNEL_POSITION_SIDE_RIGHT:
        return QStringLiteral("Side Right");
    case PA_CHANNEL_POSITION_REAR_LEFT:
        return QStringLiteral("Rear Left");
    case PA_CHANNEL_POSITION_REAR_RIGHT:
        return QStringLiteral("Rear Right");
    case PA_CHANNEL_POSITION_SUBWOOFER:
        return QStringLiteral("Subwoofer");
    default:
        return QStringLiteral("Mono");
    }
}

void Sink::testChannel(const QString &name)
{
    auto context = CanberraContext::instance()->canberra();
    if (!context)
        return;

    char dev[64];
    snprintf(dev, sizeof(dev), "%lu", (unsigned long) m_index);
    ca_context_change_device(context, dev);

    QString sound_name =  QStringLiteral("audio-channel-") + positionAsString(channelNameToPosition(name));
    ca_proplist *proplist;
    ca_proplist_create(&proplist);

    ca_proplist_sets(proplist, CA_PROP_MEDIA_ROLE, "test");
    ca_proplist_sets(proplist, CA_PROP_MEDIA_NAME, name.toLatin1().constData());
    ca_proplist_sets(proplist, CA_PROP_CANBERRA_FORCE_CHANNEL, positionAsString(channelNameToPosition(name)).toLatin1().data());
    ca_proplist_sets(proplist, CA_PROP_CANBERRA_ENABLE, "1");

    ca_proplist_sets(proplist, CA_PROP_EVENT_ID, sound_name.toLatin1().data());
    if (ca_context_play_full(context, 0, proplist, nullptr, NULL) < 0) {
        // Try a different sound name.
        ca_proplist_sets(proplist, CA_PROP_EVENT_ID, "audio-test-signal");
        if (ca_context_play_full(context, 0, proplist, nullptr, NULL) < 0) {
            // Finaly try this... if this doesn't work, then stuff it.
            ca_proplist_sets(proplist, CA_PROP_EVENT_ID, "bell-window-system");
            ca_context_play_full(context, 0, proplist, nullptr, NULL);
        }
    }

    ca_context_change_device(context, nullptr);
    ca_proplist_destroy(proplist);
}

QString Sink::positionAsString(pa_channel_position_t pos)
{
    switch (pos) {
    case PA_CHANNEL_POSITION_FRONT_LEFT:
        return QStringLiteral("front-left");
    case PA_CHANNEL_POSITION_FRONT_LEFT_OF_CENTER:
        return QStringLiteral("front-left-of-center");
    case PA_CHANNEL_POSITION_FRONT_CENTER:
        return QStringLiteral("front-center");
    case PA_CHANNEL_POSITION_MONO:
        return QStringLiteral("mono");
    case PA_CHANNEL_POSITION_FRONT_RIGHT_OF_CENTER:
        return QStringLiteral("front-right-of-center");
    case PA_CHANNEL_POSITION_FRONT_RIGHT:
        return QStringLiteral("front-right");
    case PA_CHANNEL_POSITION_SIDE_LEFT:
        return QStringLiteral("side-left");
    case PA_CHANNEL_POSITION_SIDE_RIGHT:
        return QStringLiteral("side-right");
    case PA_CHANNEL_POSITION_REAR_LEFT:
        return QStringLiteral("rear-left");
    case PA_CHANNEL_POSITION_REAR_CENTER:
        return QStringLiteral("rear-center");
    case PA_CHANNEL_POSITION_REAR_RIGHT:
        return QStringLiteral("rear-right");
    case PA_CHANNEL_POSITION_SUBWOOFER:
        return QStringLiteral("subwoofer");
    default:
        break;
    }
    return QStringLiteral("invalid");
}
} // QPulseAudio
