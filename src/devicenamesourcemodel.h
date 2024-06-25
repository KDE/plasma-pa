// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Harald Sitter <sitter@kde.org>

#pragma once

#include <QAbstractListModel>
#include <QMetaEnum>

#include "globalconfig.h"

class DeviceNameSourceModel : public QAbstractListModel
{
    Q_OBJECT
public:
    using QAbstractListModel::QAbstractListModel;

    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int intRole) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

public Q_SLOTS:
    [[nodiscard]] QString valueToProperty(int value);

private:
    [[nodiscard]] QString rowToDisplayRole(int row) const;
    QMetaEnum m_enum = QMetaEnum::fromType<GlobalConfig::EnumDeviceNameSource>();
};
