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

#ifndef CONTEXT_H
#define CONTEXT_H

#include <QMutex>
#include <QObject>
#include <QSet>

#include <pulse/mainloop.h>
#include <pulse/pulseaudio.h>
#include <pulse/glib-mainloop.h>
#include <pulse/ext-stream-restore.h>

#include "maps.h"
#include "operation.h"

namespace QPulseAudio
{

class Server;

class Q_DECL_EXPORT Context : public QObject
{
    Q_OBJECT
public:
    Context(QObject *parent = nullptr);
    ~Context();

    static Context *instance();

    static const qint64 NormalVolume = PA_VOLUME_NORM;
    static const qint64 MinimalVolume = 0;
    static const qint64 MaximalVolume = (PA_VOLUME_NORM / 100.0) * 150;

    void ref();
    void unref();

    bool isValid() { return m_context && m_mainloop; }

    const SinkMap &sinks() const { return m_sinks; }
    const SinkInputMap &sinkInputs() const { return m_sinkInputs; }
    const SourceMap &sources() const { return m_sources; }
    const SourceOutputMap &sourceOutputs() const { return m_sourceOutputs; }
    const ClientMap &clients() const { return m_clients; }
    const CardMap &cards() const { return m_cards; }
    const StreamRestoreMap &streamRestores() const { return m_streamRestores; }
    Server *server() const { return m_server; }

    void subscribeCallback(pa_context *context, pa_subscription_event_type_t type, uint32_t index);
    void contextStateCallback(pa_context *context);

    void sinkCallback(const pa_sink_info *info);
    void sinkInputCallback(const pa_sink_input_info *info);
    void sourceCallback(const pa_source_info *info);
    void sourceOutputCallback(const pa_source_output_info *info);
    void clientCallback(const pa_client_info *info);
    void cardCallback(const pa_card_info *info);
    void streamRestoreCallback(const pa_ext_stream_restore_info *info);
    void serverCallback(const pa_server_info *info);

    void setCardProfile(quint32 index, const QString &profile);
    void setDefaultSink(const QString &name);
    void setDefaultSource(const QString &name);
    void streamRestoreWrite(const pa_ext_stream_restore_info *info);

    template <typename PAFunction>
    void setGenericVolume(quint32 index, int channel, qint64 newVolume,
                          pa_cvolume cVolume, PAFunction pa_set_volume)
    {
        newVolume = qBound<qint64>(0, newVolume, PA_VOLUME_MAX);
        pa_cvolume newCVolume = cVolume;
        if (channel == -1) { // -1 all channels
            for (int i = 0; i < newCVolume.channels; ++i) {
                newCVolume.values[i] = newVolume;
            }
        } else {
            Q_ASSERT(newCVolume.channels > channel);
            newCVolume.values[channel] = newVolume;
        }
        if (!PAOperation(pa_set_volume(m_context, index, &newCVolume, nullptr, nullptr))) {
            qCWarning(PLASMAPA) <<  "pa_set_volume failed";
            return;
        }
    }

    template <typename PAFunction>
    void setGenericMute(quint32 index, bool mute, PAFunction pa_set_mute)
    {
        if (!PAOperation(pa_set_mute(m_context, index, mute, nullptr, nullptr))) {
            qCWarning(PLASMAPA) <<  "pa_set_mute failed";
            return;
        }
    }

    template <typename PAFunction>
    void setGenericPort(quint32 index, const QString &portName, PAFunction pa_set_port)
    {
        if (!PAOperation(pa_set_port(m_context,
                                     index,
                                     portName.toUtf8().constData(),
                                     nullptr,
                                     nullptr))) {
            qCWarning(PLASMAPA) << "pa_set_port failed";
            return;
        }
    }

    template <typename PAFunction>
    void setGenericDeviceForStream(quint32 streamIndex,
                                   quint32 deviceIndex,
                                   PAFunction pa_move_stream_to_device)
    {
        if (!PAOperation(pa_move_stream_to_device(m_context,
                                                  streamIndex,
                                                  deviceIndex,
                                                  nullptr,
                                                  nullptr))) {
            qCWarning(PLASMAPA) << "pa_move_stream_to_device failed";
            return;
        }
    }

private:
    void connectToDaemon();
    void reset();

    // Don't forget to add things to reset().
    SinkMap m_sinks;
    SinkInputMap m_sinkInputs;
    SourceMap m_sources;
    SourceOutputMap m_sourceOutputs;
    ClientMap m_clients;
    CardMap m_cards;
    StreamRestoreMap m_streamRestores;
    Server *m_server;

    pa_context *m_context;
    pa_glib_mainloop *m_mainloop;

    int m_references;
    static Context* s_context;
};

} // QPulseAudio

#endif // CONTEXT_H
