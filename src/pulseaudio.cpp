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
    SinkInput *sinkInput = context()->sinkInputs().data().values().at(index.row());
    Q_ASSERT(sinkInput);
    switch ((ItemRole) role) {
    case IndexRole:
        return sinkInput->index();
    case PulseObjectRole:
        return QVariant::fromValue(sinkInput);
    case NameRole:
        return sinkInput->name();
    case VolumeRole:
#warning values bs
        return sinkInput->volume().values[0];
    case SinkIndexRole:
        return sinkInput->sinkIndex();
    case IsMutedRole:
        return sinkInput->isMuted();
    case HasVolumeRole:
        return sinkInput->hasVolume();
    case IsVolumeWritableRole:
        return sinkInput->isVolumeWritable();
    case ClientIndexRole:
        Q_ASSERT(false);
    case ClientNameRole: {
        quint32 clientIndex = sinkInput->client();
        Client *client = context()->clients().data().value(clientIndex, nullptr);
        if (client)
            return client->name();
        return QVariant();
    }
    case ClientPropertiesRole: {
        quint32 clientIndex = sinkInput->client();
        Client *client = context()->clients().data().value(clientIndex, nullptr);
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

    connect(&context()->sinks(), &SinkMap::added, this, &SinkModel::volumeTextChanged);
    connect(&context()->sinks(), &SinkMap::updated, this, &SinkModel::volumeTextChanged);
    connect(&context()->sinks(), &SinkMap::removed, this, &SinkModel::volumeTextChanged);

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

int SinkModel::paIndexToDataIndex(quint32 index)
{
    qDebug() << Q_FUNC_INFO << context() << index;
    if (!context()) {
        return -1;
    }
    return context()->sinks().paIndexToDataIndex(index);
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
    case NameRole:
        return sink->name();
    case DescriptionRole:
        return sink->description();
    case VolumeRole:
        return sink->volume();
    case IsMutedRole:
        return sink->isMuted();
    case PortsRole: {
#warning this is slightly meh maybe there is a better way
        auto ports = sink->ports();
        QList<QVariant> list;
        for (auto port : ports) {
            list.append(port.toVariantMap());
        }
        return list;
    }
    case ActivePortRole:
        return sink->activePortIndex();
    }
    return QVariant();
}

QString SinkModel::volumeText() const
{
    QString ret;

    const int count = rowCount();
    for (int i = 0; i < count; ++i) {
        QString name = data(index(i), SinkModel::DescriptionRole).toString();
#warning fixme volume max as always is amiss also see qml baseitem
        int volume = data(index(i), SinkModel::VolumeRole).toInt();
        int volumePercent = 100 * volume / 65536;
#warning probably needs i18n because of percent
        if (count == 1) {
            return QString("<b>%1%</b>").arg(volumePercent);
        }
        ret.append(QString("<p>%1: <b>%2%</b></p>").arg(name, QString::number(volumePercent)));
    }

    return ret;
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

int SourceModel::paIndexToDataIndex(quint32 index)
{
    qDebug() << Q_FUNC_INFO << context() << index;
    if (!context()) {
        return -1;
    }
    return context()->sources().paIndexToDataIndex(index);
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
    case NameRole:
        return source->name();
    case DescriptionRole:
        return source->description();
    case VolumeRole:
        return source->volume();
    case IsMutedRole:
        return source->isMuted();
    case PortsRole: {
#warning this is slightly meh maybe there is a better way
        auto ports = source->ports();
        QList<QVariant> list;
        for (auto port : ports) {
            list.append(port.toVariantMap());
        }
        return list;
    }
    case ActivePortRole:
        return source->activePortIndex();
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
    case NameRole:
        return data->name();
    case SourceIndexRole:
        return data->sourceIndex();
    case VolumeRole:
        return data->volume().values[0];
    case IsMutedRole:
        return data->isMuted();
    case HasVolumeRole:
        return data->hasVolume();
    case IsVolumeWritableRole:
        return data->isVolumeWritable();
#warning code copy between SOM and SIM
    case ClientIndexRole:
        Q_ASSERT(false);
    case ClientNameRole: {
        quint32 clientIndex = data->client();
        Client *client = context()->clients().data().value(clientIndex, nullptr);
        if (client)
            return client->name();
        return QVariant();
    }
    case ClientPropertiesRole: {
        quint32 clientIndex = data->client();
        Client *client = context()->clients().data().value(clientIndex, nullptr);
        if (client)
            return client->properties();
        return QVariant();
    }
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
    Card *data =  context()->cards().data().values().at(index.row());
    Q_ASSERT(data);
    switch ((ItemRole) role) {
    case IndexRole:
        return data->index();
    case NameRole:
        return data->name();
    case DriverRole:
        return data->driver();
    case ProfilesRole: {
#warning this is slightly meh maybe there is a better way
        auto profiles = data->profiles();
        QList<QVariant> list;
        for (auto profile : profiles) {
            list.append(profile.toVariantMap());
        }
        return list;
    }
    case ActiveProfileIndexRole:
        qDebug() << "~~~~~~~~~~~~~~~" << data->activeProfileIndex();
        return data->activeProfileIndex();
    case PortsRole: {
    #warning this is slightly meh maybe there is a better way
            auto ports = data->ports();
            QList<QVariant> list;
            for (auto port : ports) {
                list.append(port.toVariantMap());
            }
            return list;
        }
    case PropertiesRole:
        return data->properties();
    }
    Q_ASSERT(false);
    return QVariant();
}
