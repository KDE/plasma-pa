#include "pulseaudio.h"

#include <QDebug>

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
    m_context = context;
    Q_ASSERT(m_context);
    Q_ASSERT(m_context->isValid());
//    connect(m_context, &Context::clientsChanged,
//            this, &ClientModel::_onDataChange);
}

QHash<int, QByteArray> ClientModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "Name";
    return roles;
}

int ClientModel::rowCount(const QModelIndex &parent) const
{
    if (!m_context)
        return 0;
    return m_context->m_clients.count();
}

QVariant ClientModel::data(const QModelIndex &index, int role) const
{
    qDebug() << Q_FUNC_INFO;
    if (role == NameRole)
        return m_context->m_clients.values().at(index.row())->name();
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
    qDebug() << Q_FUNC_INFO;
    AbstractModel::setContext(context);
    connect(context, &Context::sinkInputAdded, this, &SinkInputModel::onDataAdded);
    connect(context, &Context::sinkInputUpdated, this, &SinkInputModel::onDataUpdated);
    connect(context, &Context::sinkInputRemoved, this, &SinkInputModel::onDataRemoved);
}

QHash<int, QByteArray> SinkInputModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IndexRole] = "Index";
    roles[NameRole] = "Name";
    roles[VolumeRole] = "Volume";
    roles[HasVolumeRole] = "HasVolume";
    roles[IsVolumeWritableRole] = "IsVolumeWritable";
    roles[ClientIndexRole] = "ClientIndex";
    roles[ClientNameRole] = "ClientName";
    roles[ClientProperties] = "ClientProperties";
    return roles;
}

int SinkInputModel::rowCount(const QModelIndex &parent) const
{
//    qDebug() << Q_FUNC_INFO << parent << m_context;
    if (!m_context)
        return 0;
//    qDebug() << Q_FUNC_INFO << m_context->m_sinkInputs.count();
    return m_context->m_sinkInputs.count();
}

QVariant SinkInputModel::data(const QModelIndex &index, int role) const
{
    qDebug() << Q_FUNC_INFO << role;
    switch ((ItemRole) role) {
    case IndexRole:
        return m_context->m_sinkInputs.values().at(index.row())->index();
    case NameRole:
    case Qt::DisplayRole:
        return m_context->m_sinkInputs.values().at(index.row())->name();
    case VolumeRole:
#warning values bs
        return m_context->m_sinkInputs.values().at(index.row())->volume().values[0];
    case HasVolumeRole:
        return m_context->m_sinkInputs.values().at(index.row())->hasVolume();
    case IsVolumeWritableRole:
        return m_context->m_sinkInputs.values().at(index.row())->isVolumeWritable();
    case ClientIndexRole:
//        return
        Q_ASSERT(false);
    case ClientNameRole: {
        quint32 clientIndex = m_context->m_sinkInputs.values().at(index.row())->client();
        Client *client = m_context->m_clients.value(clientIndex, nullptr);
        if (client)
            return client->name();
        return QVariant();
    }
    case ClientProperties: {
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
    qDebug() << Q_FUNC_INFO;
    AbstractModel::setContext(context);
    connect(context, &Context::sinkAdded, this, &SinkModel::onDataAdded);
    connect(context, &Context::sinkUpdated, this, &SinkModel::onDataUpdated);
    connect(context, &Context::sinkRemoved, this, &SinkModel::onDataRemoved);
}

QHash<int, QByteArray> SinkModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IndexRole] = "Index";
    roles[NameRole] = "Name";
    roles[DescritionRole] = "Description";
    roles[VolumeRole] = "Volume";
    roles[PortsRole] = "Ports";
    roles[ActivePortRole] = "ActivePortIndex";
    return roles;
}

int SinkModel::rowCount(const QModelIndex &parent) const
{
    if (!m_context)
        return 0;
    return m_context->m_sinks.count();
}

QVariant SinkModel::data(const QModelIndex &index, int role) const
{
#warning fixme switch
    qDebug() << Q_FUNC_INFO;
    if (role == IndexRole) {
        return m_context->m_sinks.values().at(index.row())->index();
    } else if (role == NameRole) {
        return m_context->m_sinks.values().at(index.row())->name();
    } else if (role == DescritionRole) {
        return m_context->m_sinks.values().at(index.row())->description();
    } else if (role == VolumeRole) {
        return m_context->m_sinks.values().at(index.row())->volume().values[0];
    } else if (role == PortsRole) {
#warning fixme this should be a model or something or nothing, this mapping stuff here is bad
        QList<SinkPort> ports = m_context->m_sinks.values().at(index.row())->ports();
        QStringList l;
        for (SinkPort port : ports) {
            l.append(port.name());
        }
        return l;
    } else if (role == ActivePortRole) {
        return m_context->m_sinks.values().at(index.row())->activePortIndex();
    }
    return QVariant();
}

void AbstractModel::onDataAdded(quint32 index)
{
    qDebug() << Q_FUNC_INFO << index;
//    beginInsertRows(QModelIndex(), index, index);
//    endInsertRows();/
    beginResetModel();
    endResetModel();
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
