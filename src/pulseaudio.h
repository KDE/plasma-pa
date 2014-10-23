#ifndef PULSEAUDIO_H
#define PULSEAUDIO_H

#include <QAbstractListModel>
#include <QSortFilterProxyModel>

#include "context.h"

class AbstractModel : public QAbstractListModel
{
    Q_OBJECT
public:
    virtual QHash<int,QByteArray> roleNames() const Q_DECL_OVERRIDE Q_DECL_FINAL;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE = 0;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE = 0;

public slots:
    virtual void setContext(Context *context);

protected slots:
    virtual void onDataAdded(quint32 index);
    virtual void onDataUpdated(quint32 index);
    virtual void onDataRemoved(quint32 index);

protected:
    AbstractModel(QObject *parent);
    Context *m_context;

private:
    // Prevent leaf-classes from default constructing as we want to enforce
    // them passing us a context or explicit nullptrs.
    AbstractModel() {}
};

class SinkModel : public AbstractModel
{
    Q_OBJECT
    Q_PROPERTY(QString volumeText READ volumeText NOTIFY volumeTextChanged)
public:
    enum ItemRole {
        IndexRole = Qt::UserRole + 1,
        NameRole,
        DescriptionRole,
        VolumeRole,
        IsMutedRole,
        PortsRole,
        ActivePortRole
    };
    Q_ENUMS(ItemRole)

    SinkModel(Context *context = nullptr, QObject *parent = nullptr);

    Q_INVOKABLE void setContext(Context *context) Q_DECL_OVERRIDE;
#warning fixme this is so bad...
    Q_INVOKABLE int paIndexToDataIndex(quint32 index);

    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    QString volumeText() const;

signals:
    void volumeTextChanged();
};

class SourceModel : public AbstractModel
{
    Q_OBJECT
public:
    enum ItemRole {
        IndexRole = Qt::UserRole + 1,
        NameRole,
        DescriptionRole,
        VolumeRole,
        IsMutedRole,
    };
    Q_ENUMS(ItemRole)

    SourceModel(Context *context = nullptr, QObject *parent = nullptr);

    Q_INVOKABLE void setContext(Context *context) Q_DECL_OVERRIDE;
#warning fixme this is so bad...
    Q_INVOKABLE int paIndexToDataIndex(quint32 index);

    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

signals:
    void volumeTextChanged();
};

class SourceOutputModel : public AbstractModel
{
    Q_OBJECT
public:
    enum ItemRole {
        IndexRole = Qt::UserRole + 1,
        NameRole,
        SourceIndexRole,
        VolumeRole,
        IsMutedRole,
        HasVolumeRole,
        IsVolumeWritableRole,
        ClientIndexRole,
        ClientNameRole,
        ClientPropertiesRole
    };
    Q_ENUMS(ItemRole)

    SourceOutputModel(Context *context = nullptr, QObject *parent = nullptr);

    Q_INVOKABLE void setContext(Context *context) Q_DECL_OVERRIDE;

    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
};

class ClientModel : public AbstractModel
{
    Q_OBJECT
public:
    enum ItemRole {
        NameRole = Qt::UserRole + 1,
    };
    Q_ENUMS(ItemRole)

    ClientModel(Context *context = nullptr, QObject *parent = nullptr);

    Q_INVOKABLE void setContext(Context *context) Q_DECL_OVERRIDE;

    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
};

class CardModel : public AbstractModel
{
    Q_OBJECT
public:
    enum ItemRole {
        IndexRole = Qt::UserRole + 1,
        NameRole,
        DriverRole,
        ProfilesRole,
        ActiveProfileIndexRole,
        PortsRole,
        PropertiesRole
    };
    Q_ENUMS(ItemRole)

    CardModel(Context *context = nullptr, QObject *parent = nullptr);

    Q_INVOKABLE virtual void setContext(Context *context) Q_DECL_OVERRIDE;

    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
};

class SinkInputModel : public AbstractModel
{
    Q_OBJECT
public:
    enum ItemRole {
        IndexRole = Qt::UserRole + 1,
        NameRole,
        VolumeRole,
        SinkIndexRole,
        IsMutedRole,
        HasVolumeRole,
        IsVolumeWritableRole,
        ClientIndexRole,
        ClientNameRole,
        ClientPropertiesRole
    };
    Q_ENUMS(ItemRole)

    SinkInputModel(Context *context = nullptr, QObject *parent = nullptr);

    Q_INVOKABLE void setContext(Context *context) Q_DECL_OVERRIDE;

    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
};

class ReverseFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(QAbstractItemModel * sourceModel READ sourceModel WRITE setSourceModel)
public:
    ReverseFilterModel(QObject *parent = nullptr);

    Q_INVOKABLE void initialSort();
};

#endif // PULSEAUDIO_H
