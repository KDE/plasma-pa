/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>
    SPDX-FileCopyrightText: 2016 David Rosca <nowrep@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "models.h"

#include "card.h"
#include "context.h"
#include "context_p.h"
#include "debug.h"
#include "maps.h"
#include "module.h"
#include "server.h"
#include "sink.h"
#include "sinkinput.h"
#include "source.h"
#include "sourceoutput.h"
#include "streamrestore.h"

#include "models_p.h"
#include <QMetaEnum>

namespace PulseAudioQt
{
AbstractModel::AbstractModel(const MapBaseQObject *map, QObject *parent)
    : QAbstractListModel(parent)
    , d(new AbstractModelPrivate(this, map))
{
    connect(d->m_map, &MapBaseQObject::aboutToBeAdded, this, [this](int index) {
        beginInsertRows(QModelIndex(), index, index);
    });
    connect(d->m_map, &MapBaseQObject::added, this, [this](int index) {
        onDataAdded(index);
        endInsertRows();
        Q_EMIT countChanged();
    });
    connect(d->m_map, &MapBaseQObject::aboutToBeRemoved, this, [this](int index) {
        beginRemoveRows(QModelIndex(), index, index);
    });
    connect(d->m_map, &MapBaseQObject::removed, this, [this](int index) {
        Q_UNUSED(index);
        endRemoveRows();
        Q_EMIT countChanged();
    });
}

AbstractModel::~AbstractModel()
{
    delete d;
}

AbstractModelPrivate::AbstractModelPrivate(AbstractModel *q, const MapBaseQObject *map)
    : q(q)
    , m_map(map)
{
}

AbstractModelPrivate::~AbstractModelPrivate()
{
}

QHash<int, QByteArray> AbstractModel::roleNames() const
{
    if (!d->m_roles.empty()) {
        qCDebug(PULSEAUDIOQT) << "returning roles" << d->m_roles;
        return d->m_roles;
    }
    Q_UNREACHABLE();
    return QHash<int, QByteArray>();
}

int AbstractModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return d->m_map->count();
}

QVariant AbstractModel::data(const QModelIndex &index, int role) const
{
    if (!hasIndex(index.row(), index.column())) {
        return QVariant();
    }
    QObject *data = d->m_map->objectAt(index.row());
    Q_ASSERT(data);
    if (role == PulseObjectRole) {
        return QVariant::fromValue(data);
    } else if (role == Qt::DisplayRole) {
        return static_cast<PulseObject *>(data)->name();
    }
    int property = d->m_objectProperties.value(role, -1);
    if (property == -1) {
        return QVariant();
    }
    return data->metaObject()->property(property).read(data);
}

bool AbstractModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!hasIndex(index.row(), index.column())) {
        return false;
    }
    int propertyIndex = d->m_objectProperties.value(role, -1);
    if (propertyIndex == -1) {
        return false;
    }
    QObject *data = d->m_map->objectAt(index.row());
    auto property = data->metaObject()->property(propertyIndex);
    return property.write(data, value);
}

int AbstractModel::role(const QByteArray &roleName) const
{
    qCDebug(PULSEAUDIOQT) << roleName << d->m_roles.key(roleName, -1);
    return d->m_roles.key(roleName, -1);
}

Context *AbstractModel::context() const
{
    return Context::instance();
}

void AbstractModel::initRoleNames(const QMetaObject &qobjectMetaObject)
{
    d->m_roles[PulseObjectRole] = QByteArrayLiteral("PulseObject");

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
        d->m_roles[enumerator.value(i)] = key;
    }

    int maxEnumValue = -1;
    for (auto it = d->m_roles.constBegin(); it != d->m_roles.constEnd(); ++it) {
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
        d->m_roles[++maxEnumValue] = name.toLatin1();
        d->m_objectProperties.insert(maxEnumValue, i);
        if (!property.hasNotifySignal()) {
            continue;
        }
        d->m_signalIndexToProperties.insert(property.notifySignalIndex(), i);
    }
    qCDebug(PULSEAUDIOQT) << d->m_roles;

    // Connect to property changes also with objects already in model
    for (int i = 0; i < d->m_map->count(); ++i) {
        onDataAdded(i);
    }
}

void AbstractModel::propertyChanged()
{
    if (!sender() || senderSignalIndex() == -1) {
        return;
    }
    int propertyIndex = d->m_signalIndexToProperties.value(senderSignalIndex(), -1);
    if (propertyIndex == -1) {
        return;
    }
    int role = d->m_objectProperties.key(propertyIndex, -1);
    if (role == -1) {
        return;
    }
    int index = d->m_map->indexOfObject(sender());
    qCDebug(PULSEAUDIOQT) << "PROPERTY CHANGED (" << index << ") :: " << role << roleNames().value(role);
    Q_EMIT dataChanged(createIndex(index, 0), createIndex(index, 0), {role});
}

void AbstractModel::onDataAdded(int index)
{
    QObject *data = d->m_map->objectAt(index);
    const QMetaObject *mo = data->metaObject();
    // We have all the data changed notify signals already stored
    const auto keys = d->m_signalIndexToProperties.keys();
    for (const auto &index : keys) {
        QMetaMethod meth = mo->method(index);
        connect(data, meth, this, propertyChangedMetaMethod());
    }
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
    : AbstractModel(&context()->d->m_sinks, parent)
{
    initRoleNames(Sink::staticMetaObject);
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
    : AbstractModel(&context()->d->m_sources, parent)
{
    initRoleNames(Source::staticMetaObject);
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
    : AbstractModel(&context()->d->m_sinkInputs, parent)
{
    initRoleNames(SinkInput::staticMetaObject);
}

SourceOutputModel::SourceOutputModel(QObject *parent)
    : AbstractModel(&context()->d->m_sourceOutputs, parent)
{
    initRoleNames(SourceOutput::staticMetaObject);
}

CardModel::CardModel(QObject *parent)
    : AbstractModel(&context()->d->m_cards, parent)
{
    initRoleNames(Card::staticMetaObject);
}

StreamRestoreModel::StreamRestoreModel(QObject *parent)
    : AbstractModel(&context()->d->m_streamRestores, parent)
{
    initRoleNames(StreamRestore::staticMetaObject);
}

ModuleModel::ModuleModel(QObject *parent)
    : AbstractModel(&context()->d->m_modules, parent)
{
    initRoleNames(Module::staticMetaObject);
}

} // PulseAudioQt
