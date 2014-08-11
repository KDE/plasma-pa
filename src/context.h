#ifndef CONTEXT_H
#define CONTEXT_H

#include <QMap>
#include <QObject>

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
    Context();
    ~Context();

    bool isValid() { return m_context && m_mainloop; }

    void subscribeCallback(pa_context *context, pa_subscription_event_type_t type, uint32_t index);
    void contextStateCallback(pa_context *context);
    void sinkCallback(pa_context *context, const pa_sink_info *info, int eol);
    void clientCallback(pa_context *context, const pa_client_info *info, int eol);
    void sinkInputCallback(pa_context *context, const pa_sink_input_info *info, int eol);

signals:
    void sinkAdded(quint32 index);
    void sinkUpdated(quint32 index);
    void sinkRemoved(quint32 index);
//    void sinksChanged();
    void sourcesChanged();
    void sinkInputsChanged();
    void sourceOutputsChanged();
    void clientsChanged();
    void cardsChanged();

public slots:
    Q_INVOKABLE void setVolume(quint32 index, quint32 volume);

public:
//private:
    void connectToDaemon();

    QMap<qint32, Sink *> m_sinks;
//    QMap<qint32, Source *> m_sources;
    QMap<qint32, SinkInput *> m_sinkInputs;
//    QMap<qint32, SourceOutput *> m_sourceOutputs;
    QMap<qint32, Client *> m_clients;
//    QMap<qint32, Card *> m_cards;

    pa_context *m_context;
    pa_glib_mainloop *m_mainloop;
};

#endif // CONTEXT_H
