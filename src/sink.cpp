/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "sink.h"
#include "sink_p.h"

#include "context.h"
#include "context_p.h"
#include "server.h"
#include "sinkinput.h"

#include "device_p.h"
#include "volumeobject_p.h"

namespace PulseAudioQt
{
Sink::Sink(QObject *parent)
    : Device(parent)
    , d(new SinkPrivate(this))
{
    connect(Context::instance()->server(), &Server::defaultSinkChanged, this, &Sink::defaultChanged);
}

SinkPrivate::SinkPrivate(Sink *q)
    : q(q)
{
}

Sink::~Sink()
{
    delete d;
}

void SinkPrivate::update(const pa_sink_info *info)
{
    q->Device::d->updateDevice(info);

    if (m_monitorIndex != info->monitor_source) {
        m_monitorIndex = info->monitor_source;
        Q_EMIT q->monitorIndexChanged();
    }
}

void Sink::setVolume(qint64 volume)
{
    Context::instance()->d->setGenericVolume(index(), -1, volume, VolumeObject::d->cvolume(), &pa_context_set_sink_volume_by_index);
}

void Sink::setMuted(bool muted)
{
    Context::instance()->d->setGenericMute(index(), muted, &pa_context_set_sink_mute_by_index);
}

void Sink::setActivePortIndex(quint32 port_index)
{
    Port *port = qobject_cast<Port *>(ports().at(port_index));
    if (!port) {
        qCWarning(PULSEAUDIOQT) << "invalid port set request" << port_index;
        return;
    }
    Context::instance()->d->setGenericPort(index(), port->name(), &pa_context_set_sink_port_by_index);
}

void Sink::setChannelVolume(int channel, qint64 volume)
{
    Context::instance()->d->setGenericVolume(index(), channel, volume, VolumeObject::d->cvolume(), &pa_context_set_sink_volume_by_index);
}

bool Sink::isDefault() const
{
    return Context::instance()->server()->defaultSink() == this;
}

void Sink::setDefault(bool enable)
{
    if (!isDefault() && enable) {
        Context::instance()->server()->setDefaultSink(this);
    }
}

quint32 Sink::monitorIndex() const
{
    return d->m_monitorIndex;
}

void Sink::setChannelVolumes(const QVector<qint64> &channelVolumes)
{
    Context::instance()->d->setGenericVolumes(index(), channelVolumes, VolumeObject::d->m_volume, &pa_context_set_sink_volume_by_index);
}

void Sink::switchStreams()
{
    const auto sinkInputs = Context::instance()->sinkInputs();
    for (const auto &sinkInput : sinkInputs) {
        sinkInput->setDeviceIndex(index());
    }
}

} // PulseAudioQt
