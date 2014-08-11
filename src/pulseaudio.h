#ifndef PULSEAUDIO_H
#define PULSEAUDIO_H

#include <QAbstractListModel>

#include "context.h"

class Card{};

class PornModel : public QAbstractListModel
{
    Q_OBJECT
public:
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE = 0;
    virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE = 0;

public slots:
    virtual void setContext(Context *context);

private slots:
    void onDataChange();

protected:
    PornModel(QObject *parent = nullptr) : QAbstractListModel(parent), m_context(nullptr) {}

    Context *m_context;
};

class SinkModel : public PornModel
{
    Q_OBJECT
public:
    enum ItemRole {
        IndexRole = Qt::UserRole + 1,
        NameRole,
        VolumeRole
    };

    SinkModel(QObject *parent = nullptr);

    virtual void setContext(Context *context) Q_DECL_OVERRIDE;

    virtual QHash<int,QByteArray> roleNames() const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

private slots:
    void onDataAdded(quint32 index);
    void onDataUpdated(quint32 index);
    void onDataRemoved(quint32 index);
};

class ClientModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum ItemRole {
        NameRole = Qt::UserRole + 1,
    };

    ClientModel(QObject *parent = 0);
    ClientModel(Context *context, QObject *parent = 0);

    Q_INVOKABLE void setContext(Context *context);

    virtual QHash<int,QByteArray> roleNames() const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

//private slots:
//    void _onDataChange();

private:
    Context *m_context;
};

class SinkInputModel : public QAbstractListModel
{
    Q_OBJECT
public:
    SinkInputModel(Context *context, QObject *parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

private slots:
    void onDataChange();

private:
    Context *m_context;
};

#endif // PULSEAUDIO_H
