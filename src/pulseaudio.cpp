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
    qDebug() << "22222" << role;
    if (role == NameRole)
        return m_context->m_clients.values().at(index.row())->name();
    return QVariant();
}

SinkInputModel::SinkInputModel(Context *context, QObject *parent)
    : m_context(context)
{
#warning muchos duplicato
    Q_ASSERT(m_context);
    Q_ASSERT(m_context->isValid());
    connect(m_context, &Context::sinkInputsChanged,
            this, &SinkInputModel::onDataChange);
}

int SinkInputModel::rowCount(const QModelIndex &parent) const
{
    return m_context->m_sinkInputs.count();
}

QVariant SinkInputModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        return m_context->m_sinkInputs.values().at(index.row())->name();
    }
    return QVariant();
}

void SinkInputModel::onDataChange()
{
    beginResetModel();
    endResetModel();
    emit dataChanged(createIndex(0, 0), createIndex(0, rowCount()));
}

void PornModel::setContext(Context *context)
{
     beginResetModel();
     if (!m_context) {
         m_context->disconnect(this);
     }
     m_context = context;
#warning fixme meh
//     connect(m_context, &Context::sinksChanged, this, &PornModel::onDataChange);
//     endResetModel();
}

void PornModel::onDataChange()
{
//    beginResetModel();
//    endResetModel();
}


SinkModel::SinkModel(QObject *parent)
    : PornModel(parent)
{
}

void SinkModel::setContext(Context *context)
{
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
    if (role == IndexRole) {
        return m_context->m_sinks.values().at(index.row())->index();
    } else if (role == NameRole) {
        return m_context->m_sinks.values().at(index.row())->name();
    } else if (role == VolumeRole)
        return m_context->m_sinks.values().at(index.row())->volume().values[0];
    return QVariant();
}

void SinkModel::onDataAdded(quint32 index)
{
    beginInsertRows(QModelIndex(), index, index);
    endInsertRows();
}

void SinkModel::onDataUpdated(quint32 index)
{
    emit dataChanged(createIndex(index, 0), createIndex(index, 0));
}

void SinkModel::onDataRemoved(quint32 index)
{
    beginRemoveRows(QModelIndex(), index, index);
    endRemoveRows();
}
