// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Harald Sitter <sitter@kde.org>

#include "dbusinterface.h"

#include <QQmlContext>

#include "org.freedesktop.DBus.Properties.h"

void DBusInterface::classBegin()
{
}

void DBusInterface::componentComplete()
{
    auto properties = new org::freedesktop::DBus::Properties(m_service, m_path, m_bus, this);
    connect(properties,
            &org::freedesktop::DBus::Properties::PropertiesChanged,
            this,
            [this](const QString &iface, const QVariantMap &changed, [[maybe_unused]] const QStringList &invalidated) {
                if (iface != m_iface) {
                    return;
                }
                for (const auto &[key, value] : changed.asKeyValueRange()) {
                    if (!m_object->setProperty(key.toUtf8(), value)) {
                        qWarning() << "Property" << key << "not defined in qml";
                    }
                }
            });

    QVariantMap changed = properties->GetAll(m_iface);
    for (const auto &[key, value] : changed.asKeyValueRange()) {
        if (!m_object->setProperty(key.toUtf8(), value)) {
            qWarning() << "Property" << key << "not defined in qml";
        }
    }
}

void DBusInterface::asyncCall(const QString &name, const QVariantList &args, const QJSValue &resolve, const QJSValue &reject)
{
    auto method = QDBusMessage::createMethodCall(m_service, m_path, m_iface, name);
    for (const auto &arg : args) {
        method << arg;
    }
    auto reply = m_bus.asyncCall(method);
    auto watcher = new QDBusPendingCallWatcher(reply);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this, resolve, reject, watcher]() {
        watcher->deleteLater();

        auto context = QQmlEngine::contextForObject(this);
        auto engine = context->engine();
        if (watcher->reply().type() == QDBusMessage::ErrorMessage) {
            qWarning() << "Error calling method" << watcher->reply().errorMessage();
            reject.call({watcher->reply().errorMessage()});
            return;
        }

        QJSValueList args;
        for (const auto &arg : watcher->reply().arguments()) {
            args << engine->toScriptValue(arg);
        }
        resolve.call(args);
    });
}
