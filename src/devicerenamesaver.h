// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Harald Sitter <sitter@kde.org>

#pragma once

#include <QDir>
#include <QObject>

#include "devicerenamemodel.h"

class DeviceRenameSaver : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<DeviceRenameModel *> models MEMBER m_models WRITE setModels NOTIFY modelsChanged REQUIRED)
    Q_PROPERTY(bool busy MEMBER m_busy NOTIFY busyChanged)
    Q_PROPERTY(QString error MEMBER m_error NOTIFY errorChanged)
    Q_PROPERTY(bool dirty READ isDirty NOTIFY dirtyChanged)
public:
    explicit DeviceRenameSaver(QObject *parent = nullptr);
    void setModels(const QList<DeviceRenameModel *> &models);
    [[nodiscard]] bool containsOverride(const QString &name) const;
    [[nodiscard]] QVariantMap override(const QString &name) const;
    void insertOverride(const QString &name, const QVariantMap &override);
    void removeOverride(const QString &name);
    [[nodiscard]] bool isDirty() const;
    [[nodiscard]] bool containsOriginalOverride(const QString &name) const;

Q_SIGNALS:
    void modelsChanged();
    void busyChanged();
    void errorChanged();
    void dirtyChanged();

public Q_SLOTS:
    void saveChanges();

private:
    void setError(const QString &error);
    static QHash<QString, QVariantMap> readOverrides();
    bool writeOverrides(const QHash<QString, QVariantMap> &overrides);
    void restartWirePlumber();

    QList<DeviceRenameModel *> m_models;
    QHash<QString, QVariantMap> m_originalOverrides = readOverrides();
    QHash<QString, QVariantMap> m_overrides = m_originalOverrides;
    QString m_error;
    bool m_busy = false; // happens when we have data reloading pending
    QTimer m_readyTimer;
};
