#include "pulseaudio.h"

#include <QDebug>

ClientModel::ClientModel(QObject *parent)
    : QAbstractListModel(parent)
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
    qDebug() << "444444444444 roles";
    QHash<int, QByteArray> roles;
    roles[NameRole] = "meow";
    return roles;
}

int ClientModel::rowCount(const QModelIndex &parent) const
{
    if (!m_context)
        return 0;
    qDebug() << "1111 count" << m_context->m_clients.count();
    return m_context->m_clients.count();
}

QVariant ClientModel::data(const QModelIndex &index, int role) const
{
    qDebug() << Q_FUNC_INFO;
    if (role == NameRole)
        return m_context->m_clients.values().at(index.row())->name();
    return QVariant();
}

SinkInputModel::SinkInputModel(QObject *parent)
    : PornModel(parent)
{
}

void SinkInputModel::setContext(Context *context)
{
    qDebug() << Q_FUNC_INFO;
    PornModel::setContext(context);
    connect(context, &Context::sinkInputAdded, this, &SinkInputModel::onDataAdded);
    connect(context, &Context::sinkInputUpdated, this, &SinkInputModel::onDataUpdated);
    connect(context, &Context::sinkInputRemoved, this, &SinkInputModel::onDataRemoved);
}

QHash<int, QByteArray> SinkInputModel::roleNames() const
{
#warning all rolenames probably should be upcase to avoid conflict with delegate stuff in qml
    qDebug() << Q_FUNC_INFO;
    QHash<int, QByteArray> roles;
    roles[IndexRole] = "Index";
    roles[NameRole] = "Name";
    roles[VolumeRole] = "Volume";
    roles[HasVolumeRole] = "HasVolume";
    roles[IsVolumeWritableRole] = "IsVolumeWritable";
    qDebug() << roles;
    return roles;
}

int SinkInputModel::rowCount(const QModelIndex &parent) const
{
    qDebug() << Q_FUNC_INFO << parent << m_context;
    if (!m_context)
        return 0;
    qDebug() << Q_FUNC_INFO << m_context->m_sinkInputs.count();
    return m_context->m_sinkInputs.count();
}

QVariant SinkInputModel::data(const QModelIndex &index, int role) const
{
    qDebug() << Q_FUNC_INFO << role;
    switch ((ItemRole) role) {
    case IndexRole:
        return m_context->m_sinkInputs.values().at(index.row())->index();
    case NameRole:
        return m_context->m_sinkInputs.values().at(index.row())->name();
    case VolumeRole:
#warning values bs
        return m_context->m_sinkInputs.values().at(index.row())->volume().values[0];
    case HasVolumeRole:
        return m_context->m_sinkInputs.values().at(index.row())->hasVolume();
    case IsVolumeWritableRole:
        return m_context->m_sinkInputs.values().at(index.row())->isVolumeWritable();
    }
    Q_ASSERT(false);
}

void PornModel::setContext(Context *context)
{
     beginResetModel();
     if (m_context) {
         m_context->disconnect(this);
     }
     m_context = context;
     Q_ASSERT(m_context);
     endResetModel();
}

SinkModel::SinkModel(QObject *parent)
    : PornModel(parent)
{
}

void SinkModel::setContext(Context *context)
{
    qDebug() << Q_FUNC_INFO;
    PornModel::setContext(context);
    connect(context, &Context::sinkAdded, this, &SinkModel::onDataAdded);
    connect(context, &Context::sinkUpdated, this, &SinkModel::onDataUpdated);
    connect(context, &Context::sinkRemoved, this, &SinkModel::onDataRemoved);
}

QHash<int, QByteArray> SinkModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IndexRole] = "index";
    roles[NameRole] = "name";
    roles[DescritionRole] = "description";
    roles[VolumeRole] = "volume";
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
    qDebug() << Q_FUNC_INFO;
    if (role == IndexRole) {
        return m_context->m_sinks.values().at(index.row())->index();
    } else if (role == NameRole) {
        return m_context->m_sinks.values().at(index.row())->name();
    } else if (role == DescritionRole) {
        return m_context->m_sinks.values().at(index.row())->description();
    } else if (role == VolumeRole)
        return m_context->m_sinks.values().at(index.row())->volume().values[0];
    return QVariant();
}

void PornModel::onDataAdded(quint32 index)
{
    qDebug() << Q_FUNC_INFO << index;
//    beginInsertRows(QModelIndex(), index, index);
//    endInsertRows();/
    beginResetModel();
    endResetModel();
}

void PornModel::onDataUpdated(quint32 index)
{
    emit dataChanged(createIndex(index, 0), createIndex(index, 0));
}

void PornModel::onDataRemoved(quint32 index)
{
    beginRemoveRows(QModelIndex(), index, index);
    endRemoveRows();
}

PornModel::PornModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_context(nullptr)
{
}
