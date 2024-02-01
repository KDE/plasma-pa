/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>
    SPDX-FileCopyrightText: 2018 David Rosca <nowrep@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#pragma once

#include <QHash>
#include <QObject>
#include <QSet>
#include <QVector>

#include <pulse/ext-stream-restore.h>
#include <pulse/pulseaudio.h>

#include "card_p.h"
#include "client_p.h"
#include "module_p.h"
#include "sink_p.h"
#include "sinkinput_p.h"
#include "source_p.h"
#include "sourceoutput_p.h"
#include "streamrestore_p.h"

namespace PulseAudioQt
{
// Used for typedefs.
class Card;
class Client;
class Sink;
class SinkInput;
class Source;
class SourceOutput;
class StreamRestore;
class Module;

/**
 * @see MapBase
 * This class is nothing more than the QObject base since moc cannot handle
 * templates.
 */
class MapBaseQObject : public QObject
{
    Q_OBJECT

public:
    virtual int count() const = 0;
    virtual QObject *objectAt(int index) const = 0;
    virtual int indexOfObject(QObject *object) const = 0;

Q_SIGNALS:
    void aboutToBeAdded(int index);
    void added(int index, QObject *object);
    void aboutToBeRemoved(int index);
    void removed(int index, QObject *object);
};

/**
 * Maps a specific index to a specific object pointer.
 * This is used to give the unique arbitrary PulseAudio index of a PulseObject a
 * serialized list index. Namely it enables us to translate a discrete list
 * index to a pulse index to an object, and any permutation thereof.
 */
template<typename Type, typename PAInfo>
class MapBase : public MapBaseQObject
{
public:
    virtual ~MapBase()
    {
    }

    const QVector<Type *> &data() const
    {
        return m_data;
    }

    int count() const override
    {
        return m_data.count();
    }

    int indexOfObject(QObject *object) const override
    {
        return m_data.indexOf(static_cast<Type *>(object));
    }

    QObject *objectAt(int index) const override
    {
        return m_data.at(index);
    }

    void reset()
    {
        while (!m_hash.isEmpty()) {
            removeEntry(m_data.at(m_data.count() - 1)->index());
        }
        m_pendingRemovals.clear();
    }

    void insert(Type *object)
    {
        Q_ASSERT(!m_data.contains(object));

        const int modelIndex = m_data.count();

        Q_EMIT aboutToBeAdded(modelIndex);
        m_data.append(object);
        m_hash[object->index()] = object;
        Q_EMIT added(modelIndex, object);
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

        auto *obj = m_hash.value(info->index);
        if (!obj) {
            obj = new Type(parent);
            obj->d->update(info);
            insert(obj);
        } else {
            obj->d->update(info);
        }
    }

    void removeEntry(quint32 index)
    {
        if (!m_hash.contains(index)) {
            m_pendingRemovals.insert(index);
        } else {
            const int modelIndex = m_data.indexOf(m_hash.value(index));
            Q_EMIT aboutToBeRemoved(modelIndex);
            m_data.removeAt(modelIndex);
            auto object = m_hash.take(index);
            Q_EMIT removed(modelIndex, object);
            delete object;
        }
    }

protected:
    QVector<Type *> m_data;
    QHash<quint32, Type *> m_hash;
    QSet<quint32> m_pendingRemovals;
};

typedef MapBase<Card, pa_card_info> CardMap;
typedef MapBase<Client, pa_client_info> ClientMap;
typedef MapBase<SinkInput, pa_sink_input_info> SinkInputMap;
typedef MapBase<Sink, pa_sink_info> SinkMap;
typedef MapBase<Source, pa_source_info> SourceMap;
typedef MapBase<SourceOutput, pa_source_output_info> SourceOutputMap;
typedef MapBase<StreamRestore, pa_ext_stream_restore_info> StreamRestoreMap;
typedef MapBase<Module, pa_module_info> ModuleMap;

} // PulseAudioQt
