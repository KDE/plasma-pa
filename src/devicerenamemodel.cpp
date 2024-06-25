// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Harald Sitter <sitter@kde.org>

#include "devicerenamemodel.h"

#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

#include <KLocalizedString>

#include <PulseAudioQt/PulseObject>

using namespace Qt::StringLiterals;

const QString configDir = QDir::homePath() + "/.config/wireplumber/wireplumber.conf.d"_L1;
const QString configPath = configDir + "/00-plasma-pa.conf"_L1;

QHash<QString, QVariantMap> DeviceRenameModel::readOverrides()
{
    // SPA-JSON is a superset of JSON so we can simply treat things as JSON and we'll be fine.
    // We write JSON we read JSON.

    QFile file(configPath);
    if (!file.open(QFile::ReadOnly)) {
        qWarning() << "Failed to open file for reading" << file.fileName() << file.errorString();
        return {};
    }

    QJsonParseError error;
    const auto document = QJsonDocument::fromJson(file.readAll(), &error);
    if (error.error != QJsonParseError::NoError) {
        qWarning() << "Failed to parse config file" << error.error << error.errorString();
        return {};
    }

    QHash<QString, QVariantMap> data;
    const auto configObject = document.object();
    const auto rulesArray = configObject.value("monitor.alsa.rules"_L1).toArray();
#warning TODO bluetooth
    for (const auto &ruleObject : rulesArray) {
        const auto matchesArray = ruleObject["matches"_L1].toArray();
        const auto actionsObject = ruleObject["actions"_L1].toObject();

        Q_ASSERT(matchesArray.size() == 1);
        if (matchesArray.size() != 1) {
            qWarning() << "Invalid matches section" << ruleObject;
            continue;
        }
        Q_ASSERT(actionsObject.contains("update-props"_L1));
        if (!actionsObject.contains("update-props"_L1)) {
            qWarning() << "Invalid update-props section" << ruleObject;
            continue;
        }

        const auto deviceName = matchesArray.at(0).toObject()["node.name"_L1].toString();
        const auto properties = actionsObject["update-props"_L1].toObject().toVariantMap();

        Q_ASSERT(!deviceName.isEmpty());
        Q_ASSERT(!properties.isEmpty());
        if (deviceName.isEmpty() || properties.isEmpty()) {
            qWarning() << "Failed to parse rule." << deviceName.isEmpty() << properties.isEmpty() << ruleObject;
            continue;
        }

        data.insert(deviceName, properties);
    }
    qDebug() << data;
    return data;
}

void DeviceRenameModel::writeOverrides(const QHash<QString, QVariantMap> &overrides)
{
    QJsonArray rules;
    for (const auto &[name, override] : overrides.asKeyValueRange()) {
        QJsonObject rule;
        rule.insert("matches"_L1,
                     QJsonArray({QJsonObject{{"node.name"_L1, name}, //
                                             {"port.monitor"_L1, "!true"_L1}}}));
        rule.insert("actions"_L1, QJsonObject{{"update-props", QJsonObject::fromVariantMap(override)}});
        rules.append(rule);
    }

    QJsonDocument document{QJsonObject{{"monitor.alsa.rules"_L1, rules}}};
#warning TODO bluetooth
    qDebug() << document;

    if (!QDir(configDir).exists()) {
        if (!QDir().mkpath(configDir)) {
            qWarning() << "Failed to create" << configDir;
            return;
        }
    }

    QFile file(configPath);
    if (!file.open(QFile::WriteOnly | QFile::Truncate)) {
        qWarning() << "Failed to open file for writing" << file.fileName();
        return;
    }
    const auto blob = document.toJson();
    const auto written = file.write(blob);
    Q_ASSERT(written == blob.size());

    // TODO  systemctl --user restart wireplumber.service
}

int DeviceRenameModel::rowCount([[maybe_unused]] const QModelIndex &parent) const
{
    return m_sourceModel->rowCount();
}

QVariant DeviceRenameModel::data(const QModelIndex &index, int intRole) const
{
    if (!index.isValid()) {
        return {};
    }

    switch (static_cast<Role>(intRole)) {
    case Role::PulseProperties:
        const auto base = pulseProperties(index).toMap();
        const auto override = m_overrides.value(pulseName(index));
        auto result = base;
        for (const auto &[key, value] : override.asKeyValueRange()) {
            result.insert(key, value);
        }
        return result;
    }

    return {};
}

bool DeviceRenameModel::setData(const QModelIndex &index, const QVariant &value, int intRole)
{
    if (!index.isValid()) {
        return false;
    }

    switch (static_cast<Role>(intRole)) {
    case Role::PulseProperties:
        const auto name = pulseName(index);
        qWarning() << "pulsename" << name;
        if (name.isEmpty()) {
            qWarning() << "Name unexpectedly empty. Cannot override" << index;
            return false;
        }
        const auto base = pulseProperties(index).toMap();
        const auto incoming = value.toMap();
        auto override = m_overrides.value(name);
        for (const auto &[key, value] : incoming.asKeyValueRange()) {
            qDebug() << key << value;
            if (value.toString().isEmpty()) {
                // Empty means delete override
                qDebug() << "  removing";
                override.remove(key);
                continue;
            }

            if (base.value(key) == value) {
                // leave override unchanged in case we overrid things
                qDebug() << "  unchanged";
                continue;
            }

            qDebug() << "  updating";
            // If the values are not equal we override with the new value
            override.insert(key, value);
        }
        qDebug() << override;
        if (override.isEmpty()) {
            m_overrides.remove(name);
        } else {
            m_overrides.insert(name, override); // replace
        }
        qDebug() << m_overrides;
        writeOverrides(m_overrides);
        return true;
    }

    return false;
}

QHash<int, QByteArray> DeviceRenameModel::roleNames() const
{
    auto roleNames = QAbstractListModel::roleNames();
    roleNames.insert(static_cast<int>(Role::PulseProperties), "PulseProperties"_qba);
    return roleNames;
}

QString DeviceRenameModel::pulseName(const QModelIndex &index) const
{
    const auto key = m_sourceModel->roleNames().key("Name");
    return m_sourceModel->data(index, key).toString();
}

QVariant DeviceRenameModel::pulseProperties(const QModelIndex &index) const
{
    const auto key = m_sourceModel->roleNames().key("PulseProperties");
    return m_sourceModel->data(index, key);
}
