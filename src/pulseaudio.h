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
    AbstractModel(QObject *parent = nullptr);
    Context *m_context;
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

    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

signals:
    void volumeTextChanged();
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

private:
    Context *m_context;
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

class ReverseSinkInputModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    ReverseSinkInputModel(Context *context = nullptr, QObject *parent = nullptr);
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const Q_DECL_OVERRIDE;

    Q_INVOKABLE void setContext(Context *context);
};

#endif // PULSEAUDIO_H
