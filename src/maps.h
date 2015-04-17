#ifndef MAPS_H
#define MAPS_H

#include <QDebug>
#include <QMap>
#include <QObject>

#include <pulse/pulseaudio.h>

class Card;
class Client;
class Sink;
class SinkInput;
class Source;
class SourceOutput;

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

#warning this parenting here is a bit weird
    // Context is passed in as parent because context needs to include the maps
    // so we'd cause a circular dep if we were to try to use the instance here.
    // Plus that's weird separation anyway.
    void updateEntry(const PAInfo *info, QObject *parent)
    {
        Q_ASSERT(info);

        if (m_pendingRemovals.remove(info->index)) {
            // Was already removed again.
            return;
        }

        const bool isNew = !m_data.contains(info->index);

        auto *obj = m_data.value(info->index, nullptr);
        if (!obj) {
            obj = new Type(parent);
        }
        obj->update(info);
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

#endif // MAPS_H
