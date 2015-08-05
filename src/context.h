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

#include "canberra/context.h"

namespace QPulseAudio
{

class Q_DECL_EXPORT Context : public QObject
{
    Q_OBJECT
public:
    Context(QObject *parent = nullptr);
    ~Context();

    static Context *instance();

    void ref();
    void unref();

    bool isValid() { return m_context && m_mainloop; }

    const SinkMap &sinks() const { return m_sinks; }
    const SinkInputMap &sinkInputs() const { return m_sinkInputs; }
    const SourceMap &sources() const { return m_sources; }
    const SourceOutputMap &sourceOutputs() const { return m_sourceOutputs; }
    const ClientMap &clients() const { return m_clients; }
    const CardMap &cards() const { return m_cards; }

    void subscribeCallback(pa_context *context, pa_subscription_event_type_t type, uint32_t index);
    void contextStateCallback(pa_context *context);

    void sinkCallback(const pa_sink_info *info);
    void sinkInputCallback(const pa_sink_input_info *info);
    void sourceCallback(const pa_source_info *info);
    void sourceOutputCallback(const pa_source_output_info *info);
    void clientCallback(const pa_client_info *info);
    void cardCallback(const pa_card_info *info);

    void setCardProfile(quint32 index, const QString &profile);

    template <typename PAFunction>
    void setGenericVolume(quint32 index, qint64 newVolume,
                          pa_cvolume cVolume, PAFunction pa_set_volume)
    {
        // TODO: overdrive
        newVolume = qBound<qint64>(0, newVolume, 65536);
        pa_cvolume newCVolume = cVolume;
        for (int i = 0; i < newCVolume.channels; ++i) {
            newCVolume.values[i] = newVolume;
        }
        if (!PAOperation(pa_set_volume(m_context, index, &newCVolume, nullptr, nullptr))) {
            qWarning() <<  "pa_set_volume failed";
            return;
        }
#warning FIXME in the applet we edit the volume of all outputs always so audibility might be undesired
        QCanberraPrivate::Context().play(index);
    }
    template <typename PAFunction>
    void setGenericMute(quint32 index, bool mute, PAFunction pa_set_mute)
    {
        if (!PAOperation(pa_set_mute(m_context, index, mute, nullptr, nullptr))) {
            qWarning() <<  "pa_set_mute failed";
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
            qWarning() << "pa_set_port failed";
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
            qWarning() << "pa_move_stream_to_device failed";
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

    pa_context *m_context;
    pa_glib_mainloop *m_mainloop;

    int m_references;
};

} // QPulseAudio

#endif // CONTEXT_H
