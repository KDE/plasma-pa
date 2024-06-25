// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Harald Sitter <sitter@kde.org>

#pragma once

#include <QAbstractItemModel>

class DeviceRenameModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QAbstractItemModel *sourceModel MEMBER m_sourceModel NOTIFY sourceModelChanged REQUIRED)
public:
    enum class Role { PulseProperties = Qt::UserRole };
    using QAbstractListModel::QAbstractListModel;

    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int intRole) const override;
    [[nodiscard]] bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

Q_SIGNALS:
    void sourceModelChanged();

private:
    [[nodiscard]] static QHash<QString, QVariantMap> readOverrides();
    static void writeOverrides(const QHash<QString, QVariantMap> &overrides);
    [[nodiscard]] QString pulseName(const QModelIndex &index) const;
    [[nodiscard]] QVariant pulseProperties(const QModelIndex &index) const;
    QAbstractItemModel *m_sourceModel = nullptr;
    QHash<QString, QVariantMap> m_overrides = readOverrides();
};
