/*
    Copyright 2014-2015 Harald Sitter <sitter@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef PULSEAUDIO_H
#define PULSEAUDIO_H

#include <QAbstractListModel>
#include <QSortFilterProxyModel>

#include "context.h"
#include "ref.h"

namespace QPulseAudio
{

class Q_DECL_EXPORT AbstractModel : public QAbstractListModel, public Ref
{
    Q_OBJECT
public:
    virtual QHash<int,QByteArray> roleNames() const Q_DECL_OVERRIDE Q_DECL_FINAL;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE = 0;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE = 0;

    Q_INVOKABLE int role(const QByteArray &roleName) const;

protected slots:
    virtual void onDataAdded(quint32 index);
    virtual void onDataUpdated(quint32 index);
    virtual void onDataRemoved(quint32 index);

protected:
    AbstractModel(const MapBaseQObject *map, QObject *parent);
    void initRoleNames(const QMetaObject &qobjectMetaObject);

    QVariant dataForRole(QObject *obj, int role) const;

    QHash<int, QByteArray> m_roles;
    QMap<int, int> m_objectProperties;
    QMap<int, int> m_signalIndexToProperties;

private:
    // Prevent leaf-classes from default constructing as we want to enforce
    // them passing us a context or explicit nullptrs.
    AbstractModel() {}
};

class Q_DECL_EXPORT SinkModel : public AbstractModel
{
    Q_OBJECT
    Q_PROPERTY(QPulseAudio::Sink *defaultSink READ defaultSink NOTIFY defaultSinkChanged)
public:
    enum ItemRole {
        IndexRole = Qt::UserRole + 1,
        PulseObjectRole,
    };
    Q_ENUMS(ItemRole)

    SinkModel(QObject *parent = nullptr);

    Sink *defaultSink() const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    bool setData(const QModelIndex &index, const QVariant &value, int role) Q_DECL_OVERRIDE;

signals:
    void sinksChanged();
    void defaultSinkChanged();

protected:
    virtual void onDataAdded(quint32 index) Q_DECL_OVERRIDE Q_DECL_FINAL;
    virtual void onDataRemoved(quint32 index) Q_DECL_OVERRIDE Q_DECL_FINAL;

private slots:
    void propertyChanged();

private:
    QMetaMethod propertyChangedMetaMethod() const;
};

class Q_DECL_EXPORT SourceModel : public AbstractModel
{
    Q_OBJECT
    Q_PROPERTY(QPulseAudio::Source *defaultSource READ defaultSource NOTIFY defaultSourceChanged)
public:
    enum ItemRole {
        IndexRole = Qt::UserRole + 1,
        PulseObjectRole,
    };
    Q_ENUMS(ItemRole)

    SourceModel(QObject *parent = nullptr);

    Source *defaultSource() const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    bool setData(const QModelIndex &index, const QVariant &value, int role) Q_DECL_OVERRIDE;

signals:
    void sourcesChanged();
    void defaultSourceChanged();

protected:
    virtual void onDataAdded(quint32 index) Q_DECL_OVERRIDE Q_DECL_FINAL;
    virtual void onDataRemoved(quint32 index) Q_DECL_OVERRIDE Q_DECL_FINAL;

private slots:
    void propertyChanged();

private:
    QMetaMethod propertyChangedMetaMethod() const;
};

class Q_DECL_EXPORT SourceOutputModel : public AbstractModel
{
    Q_OBJECT
public:
    enum ItemRole {
        IndexRole = Qt::UserRole + 1,
        PulseObjectRole
    };
    Q_ENUMS(ItemRole)

    SourceOutputModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
};

class Q_DECL_EXPORT ClientModel : public AbstractModel
{
    Q_OBJECT
public:
    enum ItemRole {
        NameRole = Qt::UserRole + 1,
        PulseObjectRole,
    };
    Q_ENUMS(ItemRole)

    ClientModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
};

class Q_DECL_EXPORT CardModel : public AbstractModel
{
    Q_OBJECT
public:
    enum ItemRole {
        IndexRole = Qt::UserRole + 1,
        PulseObjectRole
    };
    Q_ENUMS(ItemRole)

    CardModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
};

class Q_DECL_EXPORT SinkInputModel : public AbstractModel
{
    Q_OBJECT
public:
    enum ItemRole {
        IndexRole = Qt::UserRole + 1,
        PulseObjectRole
    };
    Q_ENUMS(ItemRole)

    SinkInputModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
};

class Q_DECL_EXPORT ReverseFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(QAbstractItemModel * sourceModel READ sourceModel WRITE setSourceModel)
public:
    ReverseFilterModel(QObject *parent = nullptr);

    Q_INVOKABLE void initialSort();
};

} // QPulseAudio

#endif // PULSEAUDIO_H
