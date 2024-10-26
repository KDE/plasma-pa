// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Harald Sitter <sitter@kde.org>

#pragma once

#include <QIdentityProxyModel>
#include <QTimer>

class DeviceRenameSaver;

class DeviceRenameModel : public QIdentityProxyModel
{
    Q_OBJECT
public:
    enum class Role {
        PulseProperties = Qt::UserRole,
        HasOverride,
        HadOverride,
        Description,
        Name
    };
    using QIdentityProxyModel::QIdentityProxyModel;

    void setSaver(DeviceRenameSaver *saver);
    [[nodiscard]] QVariant data(const QModelIndex &index, int intRole) const override;
    [[nodiscard]] bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

public Q_SLOTS:
    void reset(const QModelIndex &index);

private:
    [[nodiscard]] QString pulseName(const QModelIndex &index) const;
    [[nodiscard]] QString pulseDescription(const QModelIndex &index) const;
    [[nodiscard]] QVariant pulseProperties(const QModelIndex &index) const;
    DeviceRenameSaver *m_saver = nullptr;
};
