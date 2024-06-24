// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Harald Sitter <sitter@kde.org>

#include "devicenamesourcemodel.h"

#include <KLocalizedString>

using namespace Qt::StringLiterals;

int DeviceNameSourceModel::rowCount([[maybe_unused]] const QModelIndex &parent) const
{
    return m_enum.keyCount();
}

QVariant DeviceNameSourceModel::data(const QModelIndex &index, int intRole) const
{
    if (!index.isValid()) {
        return {};
    }

    switch (intRole) {
    case Qt::DisplayRole:
        return rowToDisplayRole(index.row());
    }

    return {};
}

QHash<int, QByteArray> DeviceNameSourceModel::roleNames() const
{
    return QAbstractListModel::roleNames();
}

QString DeviceNameSourceModel::rowToDisplayRole(int row) const
{
    switch (static_cast<GlobalConfig::EnumDeviceNameSource>(row)) {
    case GlobalConfig::EnumDeviceNameSource::DeviceNick:
        return i18nc("@item:inlistbox a type of name source", "Device Nickname");
    case GlobalConfig::EnumDeviceNameSource::DeviceDescription:
        return i18nc("@item:inlistbox a type of name source", "Device Description");
    case GlobalConfig::EnumDeviceNameSource::ALSACardName:
        return i18nc("@item:inlistbox a type of name source", "ALSA Card Name");
    case GlobalConfig::EnumDeviceNameSource::ALSALongCardName:
        return i18nc("@item:inlistbox a type of name source", "ALSA Long Card Name");
    }

    Q_ASSERT_X(false, Q_FUNC_INFO, "Unexpected enum entry");
    return {};
}

QString DeviceNameSourceModel::valueToProperty(int value)
{
    switch (static_cast<GlobalConfig::EnumDeviceNameSource>(value)) {
    case GlobalConfig::EnumDeviceNameSource::DeviceNick:
        return u"node.nick"_s;
    case GlobalConfig::EnumDeviceNameSource::DeviceDescription:
        return u"node.description"_s;
    case GlobalConfig::EnumDeviceNameSource::ALSACardName:
        return u"alsa.card_name"_s;
    case GlobalConfig::EnumDeviceNameSource::ALSALongCardName:
        return u"alsa.long_card_name"_s;
    }

    Q_ASSERT_X(false, Q_FUNC_INFO, "Unexpected enum entry");
    return u"node.nick"_s;
}
