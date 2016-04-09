/*
    Copyright 2014-2015 Harald Sitter <sitter@kde.org>
    Copyright 2016 David Rosca <nowrep@gmail.com>

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

#include "pulseaudio.h"

#include "debug.h"
#include "card.h"
#include "sink.h"
#include "sinkinput.h"
#include "source.h"
#include "sourceoutput.h"
#include "server.h"
#include "streamrestore.h"

#include <QMetaEnum>

namespace QPulseAudio
{

AbstractModel::AbstractModel(const MapBaseQObject *map, QObject *parent)
    : QAbstractListModel(parent)
    , m_map(map)
{
    connect(m_map, &MapBaseQObject::added, this, &AbstractModel::onDataAdded);
    connect(m_map, &MapBaseQObject::removed, this, &AbstractModel::onDataRemoved);
}

QHash<int, QByteArray> AbstractModel::roleNames() const
{
    if (!m_roles.empty()) {
        qCDebug(PLASMAPA) << "returning roles" << m_roles;
        return m_roles;
    }
    Q_UNREACHABLE();
    return QHash<int, QByteArray>();
}

int AbstractModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_map->count();
}

QVariant AbstractModel::data(const QModelIndex &index, int role) const
{
    QObject *data = m_map->objectAt(index.row());
    Q_ASSERT(data);
    if (role == PulseObjectRole) {
        return QVariant::fromValue(data);
    }
    int property = m_objectProperties.value(role, -1);
    if (property == -1) {
        return QVariant();
    }
    return data->metaObject()->property(property).read(data);
}

bool AbstractModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    int propertyIndex = m_objectProperties.value(role, -1);
    if (propertyIndex == -1) {
        return false;
    }
    QObject *data = m_map->objectAt(index.row());
    auto property = data->metaObject()->property(propertyIndex);
    return property.write(data, value);
}

int AbstractModel::role(const QByteArray &roleName) const
{
    qCDebug(PLASMAPA) << roleName << m_roles.key(roleName, -1);
    return m_roles.key(roleName, -1);
}

void AbstractModel::initRoleNames(const QMetaObject &qobjectMetaObject)
{
    m_roles[PulseObjectRole] = QByteArrayLiteral("PulseObject");

    QMetaEnum enumerator;
    for (int i = 0; i < metaObject()->enumeratorCount(); ++i) {
        if (metaObject()->enumerator(i).name() == QLatin1String("ItemRole")) {
            enumerator = metaObject()->enumerator(i);
            break;
        }
    }

    for (int i = 0; i < enumerator.keyCount(); ++i) {
        // Clip the Role suffix and glue it in the hash.
        const int roleLength = 4;
        QByteArray key(enumerator.key(i));
        // Enum values must end in Role or the enum is crap
        Q_ASSERT(key.right(roleLength) == QByteArrayLiteral("Role"));
        key.chop(roleLength);
        m_roles[enumerator.value(i)] = key;
    }

    int maxEnumValue = -1;
    for (auto it = m_roles.constBegin(); it != m_roles.constEnd(); ++it) {
        if (it.key() > maxEnumValue) {
            maxEnumValue = it.key();
        }
    }
    Q_ASSERT(maxEnumValue != -1);
    auto mo = qobjectMetaObject;
    for (int i = 0; i < mo.propertyCount(); ++i) {
        QMetaProperty property = mo.property(i);
        QString name(property.name());
        name.replace(0, 1, name.at(0).toUpper());
        m_roles[++maxEnumValue] = name.toLatin1();
        m_objectProperties.insert(maxEnumValue, i);
        if (!property.hasNotifySignal()) {
            continue;
        }
        m_signalIndexToProperties.insert(property.notifySignalIndex(), i);
    }
    qCDebug(PLASMAPA) << m_roles;

    // Connect to property changes also with objects already in model
    for (int i = 0; i < m_map->count(); ++i) {
        onDataAdded(i);
    }
}

void AbstractModel::propertyChanged()
{
    if (!sender() || senderSignalIndex() == -1) {
        return;
    }
    int propertyIndex = m_signalIndexToProperties.value(senderSignalIndex(), -1);
    if (propertyIndex == -1) {
        return;
    }
    int role = m_objectProperties.key(propertyIndex, -1);
    if (role == -1) {
        return;
    }
    int index = m_map->indexOfObject(sender());
    qCDebug(PLASMAPA) << "PROPERTY CHANGED (" << index << ") :: " << role << roleNames().value(role);
    emit dataChanged(createIndex(index, 0), createIndex(index, 0), {role});
}

void AbstractModel::onDataAdded(int index)
{
    beginInsertRows(QModelIndex(), index, index);
    QObject *data = m_map->objectAt(index);
    const QMetaObject *mo = data->metaObject();
    // We have all the data changed notify signals already stored
    auto keys = m_signalIndexToProperties.keys();
    foreach (int index, keys) {
        QMetaMethod meth = mo->method(index);
        connect(data, meth, this, propertyChangedMetaMethod());
    }
    endInsertRows();
}

void AbstractModel::onDataRemoved(int index)
{
    beginRemoveRows(QModelIndex(), index, index);
    endRemoveRows();
}

QMetaMethod AbstractModel::propertyChangedMetaMethod() const
{
    auto mo = metaObject();
    int methodIndex = mo->indexOfMethod("propertyChanged()");
    if (methodIndex == -1) {
        return QMetaMethod();
    }
    return mo->method(methodIndex);
}

SinkModel::SinkModel(QObject *parent)
    : AbstractModel(&context()->sinks(), parent)
{
    initRoleNames(Sink::staticMetaObject);

    connect(context()->server(), &Server::defaultSinkChanged, this, &SinkModel::defaultSinkChanged);
}

Sink *SinkModel::defaultSink() const
{
    return context()->server()->defaultSink();
}

QVariant SinkModel::data(const QModelIndex &index, int role) const
{
    if (role == SortByDefaultRole) {
        // Workaround QTBUG-1548
        const QString pulseIndex = data(index, AbstractModel::role(QByteArrayLiteral("Index"))).toString();
        const QString defaultDevice = data(index, AbstractModel::role(QByteArrayLiteral("Default"))).toString();
        return defaultDevice + pulseIndex;
    }
    return AbstractModel::data(index, role);
}

SourceModel::SourceModel(QObject *parent)
    : AbstractModel(&context()->sources(), parent)
{
    initRoleNames(Source::staticMetaObject);

    connect(context()->server(), &Server::defaultSourceChanged, this, &SourceModel::defaultSourceChanged);
}

Source *SourceModel::defaultSource() const
{
    return context()->server()->defaultSource();
}

QVariant SourceModel::data(const QModelIndex &index, int role) const
{
    if (role == SortByDefaultRole) {
        // Workaround QTBUG-1548
        const QString pulseIndex = data(index, AbstractModel::role(QByteArrayLiteral("Index"))).toString();
        const QString defaultDevice = data(index, AbstractModel::role(QByteArrayLiteral("Default"))).toString();
        return defaultDevice + pulseIndex;
    }
    return AbstractModel::data(index, role);
}

SinkInputModel::SinkInputModel(QObject *parent)
    : AbstractModel(&context()->sinkInputs(), parent)
{
    initRoleNames(SinkInput::staticMetaObject);
}

SourceOutputModel::SourceOutputModel(QObject *parent)
    : AbstractModel(&context()->sourceOutputs(), parent)
{
    initRoleNames(SourceOutput::staticMetaObject);
}

CardModel::CardModel(QObject *parent)
    : AbstractModel(&context()->cards(), parent)
{
    initRoleNames(Card::staticMetaObject);
}

StreamRestoreModel::StreamRestoreModel(QObject *parent)
    : AbstractModel(&context()->streamRestores(), parent)
{
    initRoleNames(StreamRestore::staticMetaObject);
}

} // QPulseAudio
