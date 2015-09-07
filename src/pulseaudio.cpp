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

#include "pulseaudio.h"

#include "debug.h"
#include <QMetaEnum>

#include "card.h"
#include "client.h"
#include "sink.h"
#include "sinkinput.h"
#include "source.h"
#include "sourceoutput.h"

namespace QPulseAudio
{

ClientModel::ClientModel(QObject *parent)
    : AbstractModel(&context()->clients(), parent)
{
    initRoleNames(Client::staticMetaObject);
}

int ClientModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if (!context())
        return 0;
    return context()->clients().data().count();
}

QVariant ClientModel::data(const QModelIndex &index, int role) const
{
    Client *data = context()->clients().data().values().at(index.row());
    Q_ASSERT(data);
    switch(static_cast<ItemRole>(role)){
    case NameRole:
        return data->name();
    case PulseObjectRole:
        return QVariant::fromValue(data);
    }
    return dataForRole(data, role);
}

SinkInputModel::SinkInputModel(QObject *parent)
    : AbstractModel(&context()->sinkInputs(), parent)
{
    initRoleNames(SinkInput::staticMetaObject);
}

int AbstractModel::role(const QByteArray &roleName) const
{
    qCDebug(PLASMAPA) << roleName << m_roles.key(roleName, -1);
    return m_roles.key(roleName, -1);
}

int SinkInputModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if (!context())
        return 0;
    return context()->sinkInputs().data().count();
}

QVariant SinkInputModel::data(const QModelIndex &index, int role) const
{
    SinkInput *data = context()->sinkInputs().data().values().at(index.row());
    Q_ASSERT(data);
    switch ((ItemRole) role) {
    case IndexRole:
        return data->index();
    case PulseObjectRole:
        return QVariant::fromValue(data);
    }
    return dataForRole(data, role);
}

QHash<int, QByteArray> AbstractModel::roleNames() const
{
    if (!m_roles.empty()) {
        qCDebug(PLASMAPA) << "returning roles" << m_roles;
        return m_roles;
    }
    Q_ASSERT(false);
    return QHash<int, QByteArray>();
}

SinkModel::SinkModel(QObject *parent)
    : AbstractModel(&context()->sinks(), parent)
{
    initRoleNames(Sink::staticMetaObject);

    connect(&context()->sinks(), &SinkMap::added, this, &SinkModel::sinksChanged);
    connect(&context()->sinks(), &SinkMap::updated, this, &SinkModel::sinksChanged);
    connect(&context()->sinks(), &SinkMap::removed, this, &SinkModel::sinksChanged);

    emit sinksChanged();
}

#warning very naughty, used by main.qml to set volume on all sinks
QList<QObject *> SinkModel::sinks() const
{
    QList <QObject *> ret;
    if (!context())
        return ret;
    for (Sink *sink : context()->sinks().data().values()) {
        ret << sink;
    }
    return ret;
}

int SinkModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if (!context())
        return 0;
    return context()->sinks().data().count();
}

QVariant SinkModel::data(const QModelIndex &index, int role) const
{
    Sink *data = context()->sinks().data().values().at(index.row());
    Q_ASSERT(data);
    switch(static_cast<ItemRole>(role)) {
    case IndexRole:
        return data->index();
    case PulseObjectRole:
        return QVariant::fromValue(data);
    }
    return dataForRole(data, role);
}

bool SinkModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    int propertyIndex = m_objectProperties.value(role, -1);
    if (propertyIndex == -1)
        return false;
    Sink *data = context()->sinks().data().values().at(index.row());
    auto property = data->metaObject()->property(propertyIndex);
    return property.write(data, value);
}

void SinkModel::onDataAdded(quint32 index)
{
    beginInsertRows(QModelIndex(), index, index);
    Sink *data = context()->sinks().data().values().at(index);
    const QMetaObject *mo = data->metaObject();
    // We have all the data changed notify signals already stored
    auto keys = m_signalIndexToProperties.keys();
    foreach(int index, keys){
        QMetaMethod meth = mo->method(index);
        connect(data, meth, this, propertyChangedMetaMethod());
    }
    endInsertRows();
}

void SinkModel::onDataRemoved(quint32 index)
{
    beginRemoveRows(QModelIndex(), index, index);
    endRemoveRows();
}

void SinkModel::propertyChanged()
{
    if (!sender() || senderSignalIndex() == -1)
        return;
    int propertyIndex = m_signalIndexToProperties.value(senderSignalIndex(), -1);
    if (propertyIndex == -1)
        return;
    int role = m_objectProperties.key(propertyIndex, -1);
    if (role == -1)
        return;
    int index = context()->sinks().modelIndexForQObject(sender());
    qCDebug(PLASMAPA) << "PROPERTY CHANGED (" << index << ") :: " << role << roleNames().value(role);
    emit dataChanged(createIndex(index, 0), createIndex(index, 0), QVector<int>() << role);
}

QMetaMethod SinkModel::propertyChangedMetaMethod() const
{
    auto mo = metaObject();
    int methodIndex = mo->indexOfMethod(QMetaObject::normalizedSignature("propertyChanged()").data());
    if(methodIndex == -1){
        return QMetaMethod();
    }
    return mo->method(methodIndex);
}

void AbstractModel::onDataAdded(quint32 index)
{
    beginInsertRows(QModelIndex(), index, index);
    endInsertRows();
}

void AbstractModel::onDataUpdated(quint32 index)
{
    emit dataChanged(createIndex(index, 0), createIndex(index, 0));
}

void AbstractModel::onDataRemoved(quint32 index)
{
    beginRemoveRows(QModelIndex(), index, index);
    endRemoveRows();
}

AbstractModel::AbstractModel(const MapBaseQObject *map, QObject *parent)
    : QAbstractListModel(parent)
{
    connect(map, &MapBaseQObject::added, this, &AbstractModel::onDataAdded);
    connect(map, &MapBaseQObject::updated, this, &AbstractModel::onDataUpdated);
    connect(map, &MapBaseQObject::removed, this, &AbstractModel::onDataRemoved);
}

#warning probably should be called from constructor and it should get the mo passed

void AbstractModel::initRoleNames(const QMetaObject &qobjectMetaObject)
{
    QMetaEnum enumerator;
    for (int i = 0; i < metaObject()->enumeratorCount(); ++i) {
        if (metaObject()->enumerator(i).name() == QLatin1Literal("ItemRole")) {
            enumerator = metaObject()->enumerator(i);
            break;
        }
    }

    Q_ASSERT(enumerator.scope() == metaObject()->className());
    // No valid enum found, leaf probably doesn't implement ItemRole (correctly).
    Q_ASSERT(enumerator.isValid());

    for (int i = 0; i < enumerator.keyCount(); ++i) {
        // Clip the Role suffix and glue it in the hash.
        static int roleLength = strlen("Role");
        QByteArray key(enumerator.key(i));
        // Enum values must end in Role or the enum is crap
        Q_ASSERT(key.right(roleLength) == QByteArray("Role"));
        key.chop(roleLength);
        m_roles[enumerator.value(i)] = key;
    }

    int maxEnumValue = -1;
    for (auto it = m_roles.constBegin(); it != m_roles.constEnd(); ++it) {
        if (it.key() > maxEnumValue)
            maxEnumValue = it.key();
    }
    Q_ASSERT(maxEnumValue != -1);
    auto mo = qobjectMetaObject;
    for (int i = 0; i < mo.propertyCount(); ++i) {
        QMetaProperty property = mo.property(i);
        QString name(property.name());
        name.replace(0, 1, name.at(0).toUpper());
        m_roles[++maxEnumValue] = name.toLatin1();
        m_objectProperties.insert(maxEnumValue, i);
        if (!property.hasNotifySignal())
            continue;
        m_signalIndexToProperties.insert(property.notifySignalIndex(), i);
    }
    qCDebug(PLASMAPA) << m_roles;
}

QVariant AbstractModel::dataForRole(QObject *obj, int role) const
{
    int property = m_objectProperties.value(role, -1);
    if (property == -1) {
        return QVariant();
    }
    return obj->metaObject()->property(property).read(obj);
}

ReverseFilterModel::ReverseFilterModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    setDynamicSortFilter(true);
    setFilterKeyColumn(0);
}

void ReverseFilterModel::initialSort()
{
    QSortFilterProxyModel::sort(0, Qt::DescendingOrder);
}

SourceModel::SourceModel(QObject *parent)
    : AbstractModel(&context()->sources(), parent)
{
    initRoleNames(Source::staticMetaObject);

    connect(&context()->sources(), &SourceMap::added, this, &SourceModel::sourcesChanged);
    connect(&context()->sources(), &SourceMap::updated, this, &SourceModel::sourcesChanged);
    connect(&context()->sources(), &SourceMap::removed, this, &SourceModel::sourcesChanged);

    emit sourcesChanged();
}

int SourceModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if (!context())
        return 0;
    return context()->sources().data().count();
}

QVariant SourceModel::data(const QModelIndex &index, int role) const
{
    Source *data = context()->sources().data().values().at(index.row());
    Q_ASSERT(data);
    switch(static_cast<ItemRole>(role)) {
    case IndexRole:
        return data->index();
    case PulseObjectRole:
        return QVariant::fromValue(data);
    }
    return dataForRole(data, role);
}

bool SourceModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    int propertyIndex = m_objectProperties.value(role, -1);
    if (propertyIndex == -1)
        return false;
    Source *data = context()->sources().data().values().at(index.row());
    auto property = data->metaObject()->property(propertyIndex);
    return property.write(data, value);
}

void SourceModel::onDataAdded(quint32 index)
{
    beginInsertRows(QModelIndex(), index, index);
    Source *data = context()->sources().data().values().at(index);
    const QMetaObject *mo = data->metaObject();
    // We have all the data changed notify signals already stored
    auto keys = m_signalIndexToProperties.keys();
    foreach(int index, keys){
        QMetaMethod meth = mo->method(index);
        connect(data, meth, this, propertyChangedMetaMethod());
    }
    endInsertRows();
}

void SourceModel::onDataRemoved(quint32 index)
{
    beginRemoveRows(QModelIndex(), index, index);
    endRemoveRows();
}

void SourceModel::propertyChanged()
{
    if (!sender() || senderSignalIndex() == -1)
        return;
    int propertyIndex = m_signalIndexToProperties.value(senderSignalIndex(), -1);
    if (propertyIndex == -1)
        return;
    int role = m_objectProperties.key(propertyIndex, -1);
    if (role == -1)
        return;
    int index = context()->sources().modelIndexForQObject(sender());
    qCDebug(PLASMAPA) << "PROPERTY CHANGED (" << index << ") :: " << role << roleNames().value(role);
    emit dataChanged(createIndex(index, 0), createIndex(index, 0), QVector<int>() << role);
}

QMetaMethod SourceModel::propertyChangedMetaMethod() const
{
    auto mo = metaObject();
    int methodIndex = mo->indexOfMethod(QMetaObject::normalizedSignature("propertyChanged()").data());
    if(methodIndex == -1){
        return QMetaMethod();
    }
    return mo->method(methodIndex);
}

SourceOutputModel::SourceOutputModel(QObject *parent)
    : AbstractModel(&context()->sourceOutputs(), parent)
{
    initRoleNames(SourceOutput::staticMetaObject);
}

int SourceOutputModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if (!context())
        return 0;
    return context()->sourceOutputs().data().count();
}

QVariant SourceOutputModel::data(const QModelIndex &index, int role) const
{
    SourceOutput *data =  context()->sourceOutputs().data().values().at(index.row());
    Q_ASSERT(data);
    switch ((ItemRole) role) {
    case IndexRole:
        return data->index();
    case PulseObjectRole:
        return QVariant::fromValue(data);
    }
    return dataForRole(data, role);
}

CardModel::CardModel(QObject *parent)
    : AbstractModel(&context()->cards(), parent)
{
    initRoleNames(Card::staticMetaObject);
}

int CardModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if (!context())
        return 0;
    return context()->cards().data().count();
}

QVariant CardModel::data(const QModelIndex &index, int role) const
{
    Card *data =  context()->cards().data().values().at(index.row());
    Q_ASSERT(data);
    switch ((ItemRole) role) {
    case IndexRole:
        return data->index();
    case PulseObjectRole:
        return QVariant::fromValue(data);
    }
    return dataForRole(data, role);
}

} // QPulseAudio
