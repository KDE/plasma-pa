#include "pulseaudio.h"

#include <QDebug>
#include <QMetaEnum>

#include "card.h"
#include "client.h"
#include "sink.h"
#include "sinkinput.h"
#include "source.h"
#include "sourceoutput.h"

ClientModel::ClientModel(QObject *parent)
    : AbstractModel(parent)
{
    connect(&context()->clients(), &ClientMap::added, this, &ClientModel::onDataAdded);
    connect(&context()->clients(), &ClientMap::updated, this, &ClientModel::onDataUpdated);
    connect(&context()->clients(), &ClientMap::removed, this, &ClientModel::onDataRemoved);
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
    Client *client = context()->clients().data().values().at(index.row());
    Q_ASSERT(client);
    switch(static_cast<ItemRole>(role)){
    case NameRole:
        return client->name();
    case PulseObjectRole:
        return QVariant::fromValue(client);
    }
    return QVariant();
}

SinkInputModel::SinkInputModel(QObject *parent)
    : AbstractModel(parent)
{
    connect(&context()->sinkInputs(), &SinkInputMap::added, this, &SinkInputModel::onDataAdded);
    connect(&context()->sinkInputs(), &SinkInputMap::updated, this, &SinkInputModel::onDataUpdated);
    connect(&context()->sinkInputs(), &SinkInputMap::removed, this, &SinkInputModel::onDataRemoved);
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

    // Try to resolve by enum value.
    switch ((ItemRole) role) {
    case IndexRole:
        return data->index();
    case PulseObjectRole:
        return QVariant::fromValue(data);
    }

    return QVariant();
}

QHash<int, QByteArray> AbstractModel::roleNames() const
{
    if (!m_roles.empty()) {
        qDebug() << "returning roles" << m_roles;
        return m_roles;
    }

    QHash<int, QByteArray> roles;

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
        roles[enumerator.value(i)] = key;
    }

    qDebug() << roles;
    return roles;
}

SinkModel::SinkModel(QObject *parent)
    : AbstractModel(parent)
{
    connect(&context()->sinks(), &SinkMap::added, this, &SinkModel::onDataAdded);
    connect(&context()->sinks(), &SinkMap::updated, this, &SinkModel::onDataUpdated);
    connect(&context()->sinks(), &SinkMap::removed, this, &SinkModel::onDataRemoved);

    connect(&context()->sinks(), &SinkMap::added, this, &SinkModel::sinksChanged);
    connect(&context()->sinks(), &SinkMap::updated, this, &SinkModel::sinksChanged);
    connect(&context()->sinks(), &SinkMap::removed, this, &SinkModel::sinksChanged);

    emit sinksChanged();
}

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
    qDebug() << IndexRole << role;
    Sink *sink = context()->sinks().data().values().at(index.row());
    Q_ASSERT(sink);
    switch(static_cast<ItemRole>(role)) {
    case IndexRole:
        return sink->index();
    case PulseObjectRole:
        return QVariant::fromValue(sink);
    }
    return QVariant();
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

AbstractModel::AbstractModel(QObject *parent)
    : QAbstractListModel(parent)
{
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
        QString property(mo.property(i).name());
        property.replace(0, 1, property.at(0).toUpper());
        m_roles[++maxEnumValue] = property.toLatin1();
        m_objectProperties.insert(maxEnumValue, i);
    }
    qDebug() << m_roles;
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
    : AbstractModel(parent)
{
    connect(&context()->sources(), &SourceMap::added, this, &SourceModel::onDataAdded);
    connect(&context()->sources(), &SourceMap::updated, this, &SourceModel::onDataUpdated);
    connect(&context()->sources(), &SourceMap::removed, this, &SourceModel::onDataRemoved);
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
    Source *source = context()->sources().data().values().at(index.row());
    Q_ASSERT(source);
    switch(static_cast<ItemRole>(role)) {
    case IndexRole:
        return source->index();
    case PulseObjectRole:
        return QVariant::fromValue(source);
    }
    return QVariant();
}

SourceOutputModel::SourceOutputModel(QObject *parent)
    : AbstractModel(parent)
{
    connect(&context()->sourceOutputs(), &SourceOutputMap::added, this, &SourceOutputModel::onDataAdded);
    connect(&context()->sourceOutputs(), &SourceOutputMap::updated, this, &SourceOutputModel::onDataUpdated);
    connect(&context()->sourceOutputs(), &SourceOutputMap::removed, this, &SourceOutputModel::onDataRemoved);
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
    Q_ASSERT(false);
    return QVariant();
}

CardModel::CardModel(QObject *parent)
    : AbstractModel(parent)
{
    connect(&context()->cards(), &CardMap::added, this, &CardModel::onDataAdded);
    connect(&context()->cards(), &CardMap::updated, this, &CardModel::onDataUpdated);
    connect(&context()->cards(), &CardMap::removed, this, &CardModel::onDataRemoved);
}

int CardModel::rowCount(const QModelIndex &parent) const
{
    qDebug() << Q_FUNC_INFO << context() << (context() ? context()->cards().data().count() : 0);
    Q_UNUSED(parent);
    if (!context())
        return 0;
    return context()->cards().data().count();
}

QVariant CardModel::data(const QModelIndex &index, int role) const
{
    qDebug() << index<< role;
    Card *data =  context()->cards().data().values().at(index.row());
    Q_ASSERT(data);
    switch ((ItemRole) role) {
    case IndexRole:
        return data->index();
    case PulseObjectRole:
        return QVariant::fromValue(data);
    }
    Q_ASSERT(false);
    return QVariant();
}
