/*
    Copyright 2014-2015 Harald Sitter <sitter@kde.org>
    Copyright 2016 David Rosca <nowrep@gmail.com>

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

#include "maps.h"
#include "ref.h"

namespace QPulseAudio
{

class Q_DECL_EXPORT AbstractModel : public QAbstractListModel, public Ref
{
    Q_OBJECT
public:
    enum ItemRole {
        PulseObjectRole = Qt::UserRole + 1
    };

    QHash<int, QByteArray> roleNames() const Q_DECL_FINAL;
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_FINAL;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    bool setData(const QModelIndex &index, const QVariant &value, int role) Q_DECL_FINAL;

    Q_INVOKABLE int role(const QByteArray &roleName) const;

protected:
    AbstractModel(const MapBaseQObject *map, QObject *parent);
    void initRoleNames(const QMetaObject &qobjectMetaObject);

private slots:
    void propertyChanged();

private:
    void onDataAdded(int index);
    void onDataRemoved(int index);
    QMetaMethod propertyChangedMetaMethod() const;

    const MapBaseQObject *m_map;
    QHash<int, QByteArray> m_roles;
    QHash<int, int> m_objectProperties;
    QHash<int, int> m_signalIndexToProperties;

private:
    // Prevent leaf-classes from default constructing as we want to enforce
    // them passing us a context or explicit nullptrs.
    AbstractModel() {}
};

class Q_DECL_EXPORT CardModel : public AbstractModel
{
    Q_OBJECT
public:
    CardModel(QObject *parent = nullptr);
};

class Q_DECL_EXPORT SinkModel : public AbstractModel
{
    Q_OBJECT
    Q_PROPERTY(QPulseAudio::Sink *defaultSink READ defaultSink NOTIFY defaultSinkChanged)
public:
    enum ItemRole {
        SortByDefaultRole = PulseObjectRole + 1
    };
    Q_ENUMS(ItemRole)

    SinkModel(QObject *parent = nullptr);
    Sink *defaultSink() const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

signals:
    void defaultSinkChanged();
};

class Q_DECL_EXPORT SinkInputModel : public AbstractModel
{
    Q_OBJECT
public:
    SinkInputModel(QObject *parent = nullptr);
};

class Q_DECL_EXPORT SourceModel : public AbstractModel
{
    Q_OBJECT
    Q_PROPERTY(QPulseAudio::Source *defaultSource READ defaultSource NOTIFY defaultSourceChanged)
public:
    enum ItemRole {
        SortByDefaultRole = PulseObjectRole + 1
    };
    Q_ENUMS(ItemRole)

    SourceModel(QObject *parent = nullptr);
    Source *defaultSource() const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

signals:
    void defaultSourceChanged();
};

class Q_DECL_EXPORT SourceOutputModel : public AbstractModel
{
    Q_OBJECT
public:
    SourceOutputModel(QObject *parent = nullptr);
};

class Q_DECL_EXPORT StreamRestoreModel : public AbstractModel
{
    Q_OBJECT
public:
    StreamRestoreModel(QObject *parent = nullptr);
};

} // QPulseAudio

#endif // PULSEAUDIO_H
