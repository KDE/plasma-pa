/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>
    SPDX-FileCopyrightText: 2016 David Rosca <nowrep@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef PULSEAUDIO_H
#define PULSEAUDIO_H

#include <QAbstractListModel>

#include "pulseaudioqt_export.h"

namespace PulseAudioQt
{
class Context;
class MapBaseQObject;
class Sink;
class Source;
class AbstractModelPrivate;
class SinkModelPrivate;

class PULSEAUDIOQT_EXPORT AbstractModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum ItemRole { PulseObjectRole = Qt::UserRole + 1 };

    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)

    ~AbstractModel() override;
    QHash<int, QByteArray> roleNames() const final override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const final override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) final override;

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

    AbstractModelPrivate *d;

    // Prevent leaf-classes from default constructing as we want to enforce
    // them passing us a context or explicit nullptrs.
    AbstractModel()
    {
    }
};

class PULSEAUDIOQT_EXPORT CardModel : public AbstractModel
{
    Q_OBJECT
public:
    CardModel(QObject *parent = nullptr);

private:
    void *d;
};

class PULSEAUDIOQT_EXPORT SinkModel : public AbstractModel
{
    Q_OBJECT
public:
    enum ItemRole { SortByDefaultRole = PulseObjectRole + 1 };
    Q_ENUM(ItemRole)

    SinkModel(QObject *parent = nullptr);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    void *d;
};

class PULSEAUDIOQT_EXPORT SinkInputModel : public AbstractModel
{
    Q_OBJECT
public:
    SinkInputModel(QObject *parent = nullptr);

private:
    void *d;
};

class PULSEAUDIOQT_EXPORT SourceModel : public AbstractModel
{
    Q_OBJECT
public:
    enum ItemRole { SortByDefaultRole = PulseObjectRole + 1 };
    Q_ENUM(ItemRole)

    SourceModel(QObject *parent = nullptr);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    void *d;
};

class PULSEAUDIOQT_EXPORT SourceOutputModel : public AbstractModel
{
    Q_OBJECT
public:
    SourceOutputModel(QObject *parent = nullptr);

private:
    void *d;
};

class PULSEAUDIOQT_EXPORT StreamRestoreModel : public AbstractModel
{
    Q_OBJECT
public:
    StreamRestoreModel(QObject *parent = nullptr);

private:
    void *d;
};

class PULSEAUDIOQT_EXPORT ModuleModel : public AbstractModel
{
    Q_OBJECT
public:
    ModuleModel(QObject *parent = nullptr);

private:
    void *d;
};

} // PulseAudioQt

#endif // PULSEAUDIO_H
