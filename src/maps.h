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

#ifndef MAPS_H
#define MAPS_H

#include "debug.h"
#include <QMap>
#include <QObject>

#include <pulse/pulseaudio.h>

namespace QPulseAudio
{

// Used for typedefs.
class Card;
class Client;
class Sink;
class SinkInput;
class Source;
class SourceOutput;

/**
 * @see MapBase
 * This class is nothing more than the QObject base since moc cannot handle
 * templates.
 */
class Q_DECL_EXPORT MapBaseQObject : public QObject
{
    Q_OBJECT
signals:
    void added(quint32 index);
    void updated(quint32 index);
    void removed(quint32 index);
};

/**
 * Maps a specific index to a specific object pointer.
 * This is used to give the unique arbitrary PulseAudio index of a PulseObject a
 * serialized list index. Namely it enables us to translate a discrete list
 * index to a pulse index to an object, and any permutation thereof.
 */
template<typename Type, typename PAInfo>
class Q_DECL_EXPORT MapBase : public MapBaseQObject
{
public:
    virtual ~MapBase() {}

    const QMap<quint32, Type *> &data() const { return m_data; }

    int modelIndexForQObject(QObject *qobject) const
    {
        Type *t = qobject_cast<Type *>(qobject);
        if (!t)
            return -1;
        int key = m_data.key(t, -1);
        if (key == -1)
            return -1;
        return m_data.keys().indexOf(key);
    }

    /**
    * @param dataIndex index in the data model
    * @return -1 on invalid index, otherwise PA index
    */
    qint64 dataIndexToPaIndex(int dataIndex) const
    {
        auto list = m_data.values();
        qCDebug(PLASMAPA) <<  Q_FUNC_INFO << list.length() << dataIndex;
        if (list.length() <= dataIndex) {
            return -1;
        }
        qCDebug(PLASMAPA) << "  " << list.at(dataIndex)->index();
        qCDebug(PLASMAPA) << "  " << list.at(dataIndex)->name();
        return list.at(dataIndex)->index();
    }

    int paIndexToDataIndex(quint32 index) const
    {
        qCDebug(PLASMAPA) << m_data.keys() << index;
        return m_data.keys().indexOf(index);
    }

    void reset()
    {
        qDeleteAll(m_data);
        m_data.clear();
        m_pendingRemovals.clear();
    }

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

protected:
    QMap<quint32, Type *> m_data;
    QSet<quint32> m_pendingRemovals;
};

typedef MapBase<Sink, pa_sink_info> SinkMap;
typedef MapBase<SinkInput, pa_sink_input_info> SinkInputMap;
typedef MapBase<Source, pa_source_info> SourceMap;
typedef MapBase<SourceOutput, pa_source_output_info> SourceOutputMap;
typedef MapBase<Client, pa_client_info> ClientMap;
typedef MapBase<Card, pa_card_info> CardMap;

} // QPulseAudio

#endif // MAPS_H
