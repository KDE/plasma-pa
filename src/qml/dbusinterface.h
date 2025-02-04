// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Harald Sitter <sitter@kde.org>

#pragma once

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QQmlEngine>

class DBusInterface : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QString name MEMBER m_service NOTIFY nameChanged REQUIRED)
    Q_PROPERTY(QString path MEMBER m_path NOTIFY pathChanged REQUIRED)
    Q_PROPERTY(QString iface MEMBER m_iface NOTIFY ifaceChanged REQUIRED)
    Q_PROPERTY(QObject *proxy MEMBER m_object NOTIFY objectChanged REQUIRED)
public:
    using QObject::QObject;

    void classBegin() override;
    void componentComplete() override;
    Q_INVOKABLE void asyncCall(const QString &name, const QVariantList &args, const QJSValue &resolve, const QJSValue &reject);

Q_SIGNALS:
    void nameChanged();
    void pathChanged();
    void ifaceChanged();
    void objectChanged();

private:
    QDBusConnection m_bus = QDBusConnection::sessionBus();
    QString m_service;
    QString m_path;
    QString m_iface;
    QObject *m_object = nullptr;
};
