#ifndef CONTEXT_H
#define CONTEXT_H

#include <QMap>
#include <QObject>
#include <QSet>

#include <pulse/mainloop.h>
#include <pulse/pulseaudio.h>
#include <pulse/glib-mainloop.h>
#include <pulse/ext-stream-restore.h>

class Client;
class Sink;
class SinkInput;
class Source;
class SourceOutput;

template<typename Type, typename PAInfo>
class MapBase
{
public:
    virtual ~MapBase() {}

    const QMap<quint32, Type *> &data() const { return m_data; }

    void reset()
    {
        qDeleteAll(m_data);
        m_data.clear();
        m_pendingRemovals.clear();
    }

    void updateEntry(const PAInfo *info)
    {
        Q_ASSERT(info);

        if (m_pendingRemovals.remove(info->index)) {
            // Was already removed again.
            return;
        }

        const bool isNew = !m_data.contains(info->index);

        auto *obj = m_data.value(info->index, nullptr);
        if (!obj) {
            obj = new Type;
        }
        obj->setInfo(info);
        m_data.insert(info->index, obj);

        const int modelIndex = m_data.keys().indexOf(info->index);
        Q_ASSERT(modelIndex >= 0);

        if (isNew) {
            emit added(modelIndex);
        } else {
            emit updated(modelIndex);
        }
    }

    void removeEntry(quint32 index)
    {
        if (!m_data.contains(index)) {
            m_pendingRemovals.insert(index);
        } else {
            const int modelIndex = m_data.keys().indexOf(index);
            m_data.take(index)->deleteLater();
            emit removed(modelIndex);
        }
    }

    // Signals.
    virtual void added(quint32 index) = 0;
    virtual void updated(quint32 index) = 0;
    virtual void removed(quint32 index) = 0;

protected:
    QMap<quint32, Type *> m_data;
    QSet<quint32> m_pendingRemovals;
};

class SinkMap : public QObject, public MapBase<Sink, pa_sink_info>
{
    Q_OBJECT
signals:
    void added(quint32 index) Q_DECL_OVERRIDE;
    void updated(quint32 index) Q_DECL_OVERRIDE;
    void removed(quint32 index) Q_DECL_OVERRIDE;
};

class SinkInputMap : public QObject, public MapBase<SinkInput, pa_sink_input_info>
{
    Q_OBJECT
signals:
    void added(quint32 index) Q_DECL_OVERRIDE;
    void updated(quint32 index) Q_DECL_OVERRIDE;
    void removed(quint32 index) Q_DECL_OVERRIDE;
};

class SourceMap : public QObject, public MapBase<Source, pa_source_info>
{
    Q_OBJECT
signals:
    void added(quint32 index) Q_DECL_OVERRIDE;
    void updated(quint32 index) Q_DECL_OVERRIDE;
    void removed(quint32 index) Q_DECL_OVERRIDE;
};

class SourceOutputMap : public QObject, public MapBase<SourceOutput, pa_source_output_info>
{
    Q_OBJECT
signals:
    void added(quint32 index) Q_DECL_OVERRIDE;
    void updated(quint32 index) Q_DECL_OVERRIDE;
    void removed(quint32 index) Q_DECL_OVERRIDE;
};

class ClientMap : public QObject, public MapBase<Client, pa_client_info>
{
    Q_OBJECT
signals:
    void added(quint32 index) Q_DECL_OVERRIDE;
    void updated(quint32 index) Q_DECL_OVERRIDE;
    void removed(quint32 index) Q_DECL_OVERRIDE;
};

class Context : public QObject
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

    void subscribeCallback(pa_context *context, pa_subscription_event_type_t type, uint32_t index);
    void contextStateCallback(pa_context *context);

    void sinkCallback(const pa_sink_info *info);
    void sinkInputCallback(const pa_sink_input_info *info);
    void sourceCallback(const pa_source_info *info);
    void sourceOutputCallback(const pa_source_output_info *info);
    void clientCallback(const pa_client_info *info);

    Q_INVOKABLE void setSinkVolume(quint32 index, quint32 volume);
    Q_INVOKABLE void setSinkPort(quint32 portIndex);

    Q_INVOKABLE void setSinkInputVolume(quint32 index, quint32 volume);
    Q_INVOKABLE void setSinkInputSink(quint32 index, quint32 sinkIndex);

    Q_INVOKABLE void setSourceVolume(quint32 index, quint32 volume);

    Q_INVOKABLE void setSourceOutputVolume(quint32 index, quint32 volume);

private:
    void connectToDaemon();
    template <typename PAFunction>
    void setGenericVolume(quint32 index, quint32 newVolume,
                          pa_cvolume cVolume, PAFunction pa_set_volume);

    void reset();

    SinkMap m_sinks;
    SinkInputMap m_sinkInputs;
    SourceMap m_sources;
    SourceOutputMap m_sourceOutputs;
    ClientMap m_clients;

    pa_context *m_context;
    pa_glib_mainloop *m_mainloop;
};

#endif // CONTEXT_H
