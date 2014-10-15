#include "pulseaudio.h"

#include <QDebug>
#include <QMetaEnum>

ClientModel::ClientModel(QObject *parent)
    : AbstractModel(parent)
    , m_context(nullptr)
{
}

ClientModel::ClientModel(Context *context, QObject *parent)
    : ClientModel(parent)
{
    setContext(context);
}

void ClientModel::setContext(Context *context)
{
    AbstractModel::setContext(context);
    connect(context, &Context::clientAdded, this, &ClientModel::onDataAdded);
    connect(context, &Context::clientUpdated, this, &ClientModel::onDataUpdated);
    connect(context, &Context::clientRemoved, this, &ClientModel::onDataRemoved);
}

int ClientModel::rowCount(const QModelIndex &parent) const
{
    if (!m_context)
        return 0;
    return m_context->m_clients.count();
}

QVariant ClientModel::data(const QModelIndex &index, int role) const
{
    switch(static_cast<ItemRole>(role)){
    case NameRole:
        return m_context->m_clients.values().at(index.row())->name();
    }
    return QVariant();
}

SinkInputModel::SinkInputModel(Context *context, QObject *parent)
    : AbstractModel(parent)
{
    if (context) {
        setContext(context);
    }
}

void SinkInputModel::setContext(Context *context)
{
    AbstractModel::setContext(context);
    connect(context, &Context::sinkInputAdded, this, &SinkInputModel::onDataAdded);
    connect(context, &Context::sinkInputUpdated, this, &SinkInputModel::onDataUpdated);
    connect(context, &Context::sinkInputRemoved, this, &SinkInputModel::onDataRemoved);
}

int SinkInputModel::rowCount(const QModelIndex &parent) const
{
    if (!m_context)
        return 0;
    return m_context->m_sinkInputs.count();
}

QVariant SinkInputModel::data(const QModelIndex &index, int role) const
{
    switch ((ItemRole) role) {
    case IndexRole:
        return m_context->m_sinkInputs.values().at(index.row())->index();
    case NameRole:
        return m_context->m_sinkInputs.values().at(index.row())->name();
    case VolumeRole:
#warning values bs
        return m_context->m_sinkInputs.values().at(index.row())->volume().values[0];
    case IsMutedRole:
        return m_context->m_sinkInputs.values().at(index.row())->isMuted();
    case HasVolumeRole:
        return m_context->m_sinkInputs.values().at(index.row())->hasVolume();
    case IsVolumeWritableRole:
        return m_context->m_sinkInputs.values().at(index.row())->isVolumeWritable();
    case ClientIndexRole:
        Q_ASSERT(false);
    case ClientNameRole: {
        quint32 clientIndex = m_context->m_sinkInputs.values().at(index.row())->client();
        Client *client = m_context->m_clients.value(clientIndex, nullptr);
        if (client)
            return client->name();
        return QVariant();
    }
    case ClientPropertiesRole: {
        quint32 clientIndex = m_context->m_sinkInputs.values().at(index.row())->client();
        Client *client = m_context->m_clients.value(clientIndex, nullptr);
        if (client)
            return client->properties();
        return QVariant();
    }
    }
    return QVariant();
    Q_ASSERT(false);
}

QHash<int, QByteArray> AbstractModel::roleNames() const
{
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
        static auto roleLength = strlen("Role");
        QByteArray key(enumerator.key(i));
        // Enum values must end in Role or the enum is crap
        Q_ASSERT(key.right(roleLength) == QByteArray("Role"));
        key.chop(roleLength);
        roles[enumerator.value(i)] = key;
    }

    qDebug() << roles;
    return roles;
}

void AbstractModel::setContext(Context *context)
{
     beginResetModel();
     if (m_context) {
         m_context->disconnect(this);
     }
     m_context = context;
     Q_ASSERT(m_context);
     endResetModel();
}

SinkModel::SinkModel(Context *context, QObject *parent)
    : AbstractModel(parent)
{
    if (context) {
        setContext(context);
    }
}

void SinkModel::setContext(Context *context)
{
    AbstractModel::setContext(context);
    connect(context, &Context::sinkAdded, this, &SinkModel::onDataAdded);
    connect(context, &Context::sinkUpdated, this, &SinkModel::onDataUpdated);
    connect(context, &Context::sinkRemoved, this, &SinkModel::onDataRemoved);
}

int SinkModel::rowCount(const QModelIndex &parent) const
{
    if (!m_context)
        return 0;
    return m_context->m_sinks.count();
}

QVariant SinkModel::data(const QModelIndex &index, int role) const
{
    switch(static_cast<ItemRole>(role)) {
    case IndexRole:
        return m_context->m_sinks.values().at(index.row())->index();
    case NameRole:
        return m_context->m_sinks.values().at(index.row())->name();
    case DescriptionRole:
        return m_context->m_sinks.values().at(index.row())->description();
    case VolumeRole:
        return m_context->m_sinks.values().at(index.row())->volume().values[0];
    case IsMutedRole:
        return m_context->m_sinks.values().at(index.row())->isMuted();
    case PortsRole: {
#warning fixme this should be a model or something or nothing, this mapping stuff here is bad
        QList<SinkPort> ports = m_context->m_sinks.values().at(index.row())->ports();
        QStringList l;
        for (SinkPort port : ports) {
            l.append(port.name());
        }
        return l;
    }
    case ActivePortRole:
        return m_context->m_sinks.values().at(index.row())->activePortIndex();
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
    , m_context(nullptr)
{
}
