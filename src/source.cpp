/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "source.h"
#include "source_p.h"

#include "context.h"
#include "context_p.h"
#include "device_p.h"
#include "server.h"
#include "volumeobject_p.h"

namespace PulseAudioQt
{
Source::Source(QObject *parent)
    : Device(parent)
    , d(new SourcePrivate(this))
{
    connect(Context::instance()->server(), &Server::defaultSourceChanged, this, &Source::defaultChanged);
}

SourcePrivate::SourcePrivate(Source *q)
    : q(q)
{
}

void SourcePrivate::update(const pa_source_info *info)
{
    q->Device::d->updateDevice(info);
}

void Source::setVolume(qint64 volume)
{
    Context::instance()->d->setGenericVolume(index(), -1, volume, VolumeObject::d->cvolume(), &pa_context_set_source_volume_by_index);
}

void Source::setMuted(bool muted)
{
    Context::instance()->d->setGenericMute(index(), muted, &pa_context_set_source_mute_by_index);
}

void Source::setActivePortIndex(quint32 port_index)
{
    Port *port = qobject_cast<Port *>(ports().at(port_index));
    if (!port) {
        qCWarning(PULSEAUDIOQT) << "invalid port set request" << port_index;
        return;
    }
    Context::instance()->d->setGenericPort(index(), port->name(), &pa_context_set_source_port_by_index);
}

void Source::setChannelVolume(int channel, qint64 volume)
{
    Context::instance()->d->setGenericVolume(index(), channel, volume, VolumeObject::d->cvolume(), &pa_context_set_source_volume_by_index);
}

bool Source::isDefault() const
{
    return Context::instance()->server()->defaultSource() == this;
}

void Source::setDefault(bool enable)
{
    if (!isDefault() && enable) {
        Context::instance()->server()->setDefaultSource(this);
    }
}

void Source::switchStreams()
{
    const auto sourceOutputs = Context::instance()->sourceOutputs();
    for (const auto &sourceOutput : sourceOutputs) {
        sourceOutput->setDeviceIndex(index());
    }
}

void Source::setChannelVolumes(const QVector<qint64> &volumes)
{
    Context::instance()->d->setGenericVolumes(index(), volumes, VolumeObject::d->m_volume, &pa_context_set_source_volume_by_index);
}

Source::~Source()
{
    delete d;
}
} // PulseAudioQt
