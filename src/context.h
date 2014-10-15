#ifndef CONTEXT_H
#define CONTEXT_H

#include <QMap>
#include <QObject>
#include <QSet>

#include <pulse/mainloop.h>
#include <pulse/pulseaudio.h>
#include <pulse/glib-mainloop.h>
#include <pulse/ext-stream-restore.h>

#include "client.h"
#include "sink.h"
#include "sinkinput.h"

class Context : public QObject
{
    Q_OBJECT
public:
    Context(QObject *parent = nullptr);
    ~Context();

    bool isValid() { return m_context && m_mainloop; }

    void subscribeCallback(pa_context *context, pa_subscription_event_type_t type, uint32_t index);
    void contextStateCallback(pa_context *context);

    template <typename C, typename PAInfo, typename AddedSignal, typename UpdatedSignal>
    void updateMap(QMap<quint32, C *> &map,
                   QSet<quint32> &exclusionSet,
                   const PAInfo *info,
                   AddedSignal addedSignal,
                   UpdatedSignal updatedSignal);

    void sinkCallback(pa_context *context, const pa_sink_info *info, int eol);
    void clientCallback(pa_context *context, const pa_client_info *info, int eol);
    void sinkInputCallback(pa_context *context, const pa_sink_input_info *info, int eol);

    Q_INVOKABLE void setSinkVolume(quint32 index, quint32 volume);
    Q_INVOKABLE void setSinkPort(quint32 portIndex);
    Q_INVOKABLE void setSinkInputVolume(quint32 index, quint32 volume);

signals:
    void sinkAdded(quint32 index);
    void sinkUpdated(quint32 index);
    void sinkRemoved(quint32 index);

    void sinkInputAdded(quint32 index);
    void sinkInputUpdated(quint32 index);
    void sinkInputRemoved(quint32 index);

    void clientAdded(quint32 index);
    void clientUpdated(quint32 index);
    void clientRemoved(quint32 index);

public:
//private:
    void connectToDaemon();

    QMap<quint32, Sink *> m_sinks;
    QSet<quint32> m_recentlyDeletedSinks;
//    QMap<quint32, Source *> m_sources;
    QMap<quint32, SinkInput *> m_sinkInputs;
    QSet<quint32> m_recentlyDeletedSinkInputs;
//    QMap<quint32, SourceOutput *> m_sourceOutputs;
    QMap<quint32, Client *> m_clients;
    QSet<quint32> m_recentDeletedClients;
//    QMap<quint32, Card *> m_cards;

    pa_context *m_context;
    pa_glib_mainloop *m_mainloop;
};

#endif // CONTEXT_H
