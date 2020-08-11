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

namespace QPulseAudio
{
    class Context;

class AbstractModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum ItemRole {
        PulseObjectRole = Qt::UserRole + 1
    };
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)

    Q_ENUM(ItemRole)

    ~AbstractModel() override;
    QHash<int, QByteArray> roleNames() const final;
    int rowCount(const QModelIndex &parent = QModelIndex()) const final;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) final;

    Q_INVOKABLE int role(const QByteArray &roleName) const;

Q_SIGNALS:
    void countChanged();

protected:
    AbstractModel(const MapBaseQObject *map, QObject *parent);
    void initRoleNames(const QMetaObject &qobjectMetaObject);
    Context *context() const;

private Q_SLOTS:
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

class CardModel : public AbstractModel
{
    Q_OBJECT
public:
    explicit CardModel(QObject *parent = nullptr);
};

class SinkModel : public AbstractModel
{
    Q_OBJECT
    Q_PROPERTY(QPulseAudio::Sink *defaultSink READ defaultSink NOTIFY defaultSinkChanged)
    Q_PROPERTY(QPulseAudio::Sink *preferredSink READ preferredSink NOTIFY preferredSinkChanged)
public:
    enum ItemRole {
        SortByDefaultRole = PulseObjectRole + 1
    };
    Q_ENUM(ItemRole)

    explicit SinkModel(QObject *parent = nullptr);
    Sink *defaultSink() const;
    Sink *preferredSink() const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

Q_SIGNALS:
    void defaultSinkChanged();
    void preferredSinkChanged();

private:
    void sinkAdded(int index);
    void sinkRemoved(int index);
    void updatePreferredSink();
    Sink *findPreferredSink() const;

    Sink *m_preferredSink;
};

class SinkInputModel : public AbstractModel
{
    Q_OBJECT
public:
    explicit SinkInputModel(QObject *parent = nullptr);
};

class SourceModel : public AbstractModel
{
    Q_OBJECT
    Q_PROPERTY(QPulseAudio::Source *defaultSource READ defaultSource NOTIFY defaultSourceChanged)
public:
    enum ItemRole {
        SortByDefaultRole = PulseObjectRole + 1
    };
    Q_ENUM(ItemRole)

    explicit SourceModel(QObject *parent = nullptr);
    Source *defaultSource() const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

Q_SIGNALS:
    void defaultSourceChanged();
};

class SourceOutputModel : public AbstractModel
{
    Q_OBJECT
public:
    explicit SourceOutputModel(QObject *parent = nullptr);
};

class StreamRestoreModel : public AbstractModel
{
    Q_OBJECT
public:
    explicit StreamRestoreModel(QObject *parent = nullptr);
};

class ModuleModel : public AbstractModel
{
    Q_OBJECT
public:
    explicit ModuleModel(QObject *parent = nullptr);
};

} // QPulseAudio

#endif // PULSEAUDIO_H
