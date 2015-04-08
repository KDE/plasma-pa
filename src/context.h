#ifndef CONTEXT_H
#define CONTEXT_H

#include <QMap>
#include <QObject>
#include <QSet>

#include <pulse/mainloop.h>
#include <pulse/pulseaudio.h>
#include <pulse/glib-mainloop.h>
#include <pulse/ext-stream-restore.h>

class Card;
class Client;
class Sink;
class SinkInput;
class Source;
class SourceOutput;

#include <QDebug>

template<typename Type, typename PAInfo>
class Q_DECL_EXPORT MapBase
{
public:
    virtual ~MapBase() {}

    const QMap<quint32, Type *> &data() const { return m_data; }

    /**
     * @param dataIndex index in the data model
     * @return -1 on invalid index, otherwise PA index
     */
    qint64 dataIndexToPaIndex(int dataIndex) const
    {
        auto list = m_data.values();
        qDebug() <<  Q_FUNC_INFO << list.length() << dataIndex;
        if (list.length() <= dataIndex) {
            return -1;
        }
        qDebug() << "  " << list.at(dataIndex)->index();
        qDebug() << "  " << list.at(dataIndex)->name();
        return list.at(dataIndex)->index();
    }

    int paIndexToDataIndex(quint32 index) const
    {
        qDebug() << m_data.keys() << index;
        return m_data.keys().indexOf(index);
    }

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

class Q_DECL_EXPORT SinkMap : public QObject, public MapBase<Sink, pa_sink_info>
{
    Q_OBJECT
signals:
    void added(quint32 index) Q_DECL_OVERRIDE;
    void updated(quint32 index) Q_DECL_OVERRIDE;
    void removed(quint32 index) Q_DECL_OVERRIDE;
};

class Q_DECL_EXPORT SinkInputMap : public QObject, public MapBase<SinkInput, pa_sink_input_info>
{
    Q_OBJECT
signals:
    void added(quint32 index) Q_DECL_OVERRIDE;
    void updated(quint32 index) Q_DECL_OVERRIDE;
    void removed(quint32 index) Q_DECL_OVERRIDE;
};

class Q_DECL_EXPORT SourceMap : public QObject, public MapBase<Source, pa_source_info>
{
    Q_OBJECT
signals:
    void added(quint32 index) Q_DECL_OVERRIDE;
    void updated(quint32 index) Q_DECL_OVERRIDE;
    void removed(quint32 index) Q_DECL_OVERRIDE;
};

class Q_DECL_EXPORT SourceOutputMap : public QObject, public MapBase<SourceOutput, pa_source_output_info>
{
    Q_OBJECT
signals:
    void added(quint32 index) Q_DECL_OVERRIDE;
    void updated(quint32 index) Q_DECL_OVERRIDE;
    void removed(quint32 index) Q_DECL_OVERRIDE;
};

class Q_DECL_EXPORT ClientMap : public QObject, public MapBase<Client, pa_client_info>
{
    Q_OBJECT
signals:
    void added(quint32 index) Q_DECL_OVERRIDE;
    void updated(quint32 index) Q_DECL_OVERRIDE;
    void removed(quint32 index) Q_DECL_OVERRIDE;
};

class Q_DECL_EXPORT CardMap : public QObject, public MapBase<Card, pa_card_info>
{
    Q_OBJECT
signals:
    void added(quint32 index) Q_DECL_OVERRIDE;
    void updated(quint32 index) Q_DECL_OVERRIDE;
    void removed(quint32 index) Q_DECL_OVERRIDE;
};

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

    Q_INVOKABLE void setSinkVolume(quint32 index, quint32 volume);
    Q_INVOKABLE void setSinkPort(quint32 index, const QString &portName);

    Q_INVOKABLE void setSinkInputVolume(quint32 index, quint32 volume);
#warning terminology is messed up... modelIndex is a QModelIndex, what we have here is a MapIndex...
    Q_INVOKABLE void setSinkInputSinkByModelIndex(quint32 index, int sinkModelIndex);

    Q_INVOKABLE void setSourceVolume(quint32 index, quint32 volume);

    Q_INVOKABLE void setSourceOutputVolume(quint32 index, quint32 volume);
    Q_INVOKABLE void setSourceOutputSinkByModelIndex(quint32 index, int sourceModelIndex);

    Q_INVOKABLE void setCardProfile(quint32 cardIndex, const QString &profileName);

private:
    void connectToDaemon();
    template <typename PAFunction>
    void setGenericVolume(quint32 index, quint32 newVolume,
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

#endif // CONTEXT_H
