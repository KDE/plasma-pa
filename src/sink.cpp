/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "sink.h"

#include "canberracontext.h"
#include "context.h"
#include "server.h"
#include "sinkinput.h"

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
    if (m_sourceIndex != info->monitor_source) {
        m_sourceIndex = info->monitor_source;
        Q_EMIT sourceIndexChanged();
    }
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

void Sink::setChannelVolumes(const QVector<qint64> &channelVolumes)
{
    context()->setGenericVolumes(index(), channelVolumes, cvolume(), &pa_context_set_sink_volume_by_index);
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

void Sink::testChannel(const QString &name)
{
    auto context = CanberraContext::instance()->canberra();
    if (!context) {
        return;
    }

    ca_context_set_driver(context, "pulse");

    char dev[64];
    snprintf(dev, sizeof(dev), "%lu", (unsigned long)m_index);
    ca_context_change_device(context, dev);

    QString sound_name = QStringLiteral("audio-channel-") + name;
    ca_proplist *proplist;
    ca_proplist_create(&proplist);

    ca_proplist_sets(proplist, CA_PROP_MEDIA_ROLE, "test");
    ca_proplist_sets(proplist, CA_PROP_MEDIA_NAME, name.toLatin1().constData());
    ca_proplist_sets(proplist, CA_PROP_CANBERRA_FORCE_CHANNEL, name.toLatin1().data());
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

void Sink::switchStreams()
{
    auto data = context()->sinkInputs().data();
    std::for_each(data.begin(), data.end(), [this](SinkInput *paObj) {
        paObj->setDeviceIndex(m_index);
    });
}

quint32 Sink::sourceIndex() const
{
    return m_sourceIndex;
}

} // QPulseAudio
