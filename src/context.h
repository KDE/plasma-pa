#ifndef CONTEXT_H
#define CONTEXT_H

#include <QObject>
#include <QSet>

#include <pulse/mainloop.h>
#include <pulse/pulseaudio.h>
#include <pulse/glib-mainloop.h>
#include <pulse/ext-stream-restore.h>

#include "maps.h"

class Q_DECL_EXPORT Context : public QObject
{
    Q_OBJECT
public:
    Context(QObject *parent = nullptr);
    ~Context();

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

    Q_INVOKABLE void setSinkVolume(quint32 index, qint64 volume);
    Q_INVOKABLE void setSinkMute(quint32 index, bool mute);
    Q_INVOKABLE void setSinkPort(quint32 index, const QString &portName);

    Q_INVOKABLE void setSinkInputVolume(quint32 index, qint64 volume);
#warning terminology is messed up... modelIndex is a QModelIndex, what we have here is a MapIndex...
    Q_INVOKABLE void setSinkInputSinkByModelIndex(quint32 index, int sinkModelIndex);

    Q_INVOKABLE void setSourceVolume(quint32 index, qint64 volume);

    Q_INVOKABLE void setSourceOutputVolume(quint32 index, qint64 volume);
    Q_INVOKABLE void setSourceOutputSinkByModelIndex(quint32 index, int sourceModelIndex);

    Q_INVOKABLE void setCardProfile(quint32 cardIndex, const QString &profileName);

    template <typename PAFunction>
    void setGenericMute(quint32 index, bool mute, PAFunction pa_set_mute);

private:
    void connectToDaemon();
    template <typename PAFunction>
    void setGenericVolume(quint32 index, qint64 newVolume,
                          pa_cvolume cVolume, PAFunction pa_set_volume);

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
};

Q_GLOBAL_STATIC(Context, s_context)

#endif // CONTEXT_H
