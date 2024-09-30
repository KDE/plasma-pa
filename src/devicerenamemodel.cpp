// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Harald Sitter <sitter@kde.org>

#include "devicerenamemodel.h"

#include "debug.h"
#include "devicerenamesaver.h"

using namespace Qt::StringLiterals;

QVariant DeviceRenameModel::data(const QModelIndex &index, int intRole) const
{
    if (!checkIndex(index, CheckIndexOption::IndexIsValid | CheckIndexOption::ParentIsInvalid)) {
        return {};
    }

    switch (static_cast<Role>(intRole)) {
    case Role::HadOverride: {
        const auto name = pulseName(index);
        if (name.isEmpty()) {
            qWarning() << "Name unexpectedly empty. Cannot override" << index;
            return false;
        }
        return m_saver->containsOriginalOverride(name);
    }
    case Role::HasOverride: {
        const auto name = pulseName(index);
        if (name.isEmpty()) {
            qWarning() << "Name unexpectedly empty. Cannot override" << index;
            return false;
        }
        return m_saver->containsOverride(name);
    }
    case Role::PulseProperties: {
        const auto base = pulseProperties(index).toMap();
        const auto override = m_saver->override(pulseName(index));
        auto result = base;
        for (const auto &[key, value] : override.asKeyValueRange()) {
            result.insert(key, value);
        }
        return result;
    }
    case Role::Description: {
        const auto description = pulseDescription(index);
        if (description.isEmpty()) {
            qWarning() << "Description unexpectedly empty" << index;
            return false;
        }
        return description;
    }
    case Role::Name: {
        const auto name = pulseName(index);
        if (name.isEmpty()) {
            qWarning() << "Name unexpectedly empty" << index;
            return false;
        }
        return name;
    }
    }

    return QIdentityProxyModel::data(index, intRole);
}

bool DeviceRenameModel::setData(const QModelIndex &index, const QVariant &value, int intRole)
{
    if (!index.isValid()) {
        return false;
    }

    switch (static_cast<Role>(intRole)) {
    case Role::HasOverride:
    case Role::HadOverride:
    case Role::Description:
    case Role::Name:
        qCWarning(PLASMAPA) << "Cannot write to role.";
        return false;
    case Role::PulseProperties:
        const auto name = pulseName(index);
        if (name.isEmpty()) {
            qCWarning(PLASMAPA) << "Name unexpectedly empty. Cannot override" << index;
            return false;
        }
        beginResetModel();
        const auto base = pulseProperties(index).toMap();
        const auto incoming = value.toMap();
        auto override = m_saver->override(name);
        for (const auto &[key, value] : incoming.asKeyValueRange()) {
            if (value.toString().isEmpty()) {
                // Empty means delete override
                override.remove(key);
                continue;
            }

            if (base.value(key) == value) {
                // leave override unchanged in case we overrid things
                continue;
            }

            // If the values are not equal we override with the new value
            override.insert(key, value);
        }
        if (override.isEmpty()) {
            m_saver->removeOverride(name);
        } else {
            m_saver->insertOverride(name, override); // replace
        }
        endResetModel();
        return true;
    }

    return false;
}

QHash<int, QByteArray> DeviceRenameModel::roleNames() const
{
    // Do not start out with the original role names. We'd get confused over what
    // is what since this is an IdentityProxy and churns through roles from the source model
    QHash<int, QByteArray> roleNames;
    roleNames.insert(static_cast<int>(Role::PulseProperties), "PulseProperties"_ba);
    roleNames.insert(static_cast<int>(Role::HasOverride), "HasOverride"_ba);
    roleNames.insert(static_cast<int>(Role::HadOverride), "HadOverride"_ba);
    roleNames.insert(static_cast<int>(Role::Description), "Description"_ba);
    roleNames.insert(static_cast<int>(Role::Name), "Name"_ba);
    return roleNames;
}

QString DeviceRenameModel::pulseName(const QModelIndex &index) const
{
    const auto key = sourceModel()->roleNames().key("Name");
    return QIdentityProxyModel::data(index, key).toString();
}

QString DeviceRenameModel::pulseDescription(const QModelIndex &index) const
{
    const auto key = sourceModel()->roleNames().key("Description");
    return QIdentityProxyModel::data(index, key).toString();
}

QVariant DeviceRenameModel::pulseProperties(const QModelIndex &index) const
{
    const auto key = sourceModel()->roleNames().key("PulseProperties");
    return QIdentityProxyModel::data(index, key);
}

void DeviceRenameModel::reset(const QModelIndex &index)
{
    if (!checkIndex(index, CheckIndexOption::IndexIsValid | CheckIndexOption::ParentIsInvalid)) {
        return;
    }

    const auto name = pulseName(index);
    if (name.isEmpty()) {
        qCWarning(PLASMAPA) << "Name unexpectedly empty. Cannot override" << index;
        return;
    }

    beginResetModel();
    m_saver->removeOverride(name);
    endResetModel();
}

void DeviceRenameModel::setSaver(DeviceRenameSaver *saver)
{
    if (m_saver == saver) {
        return;
    }
    m_saver = saver;
}
