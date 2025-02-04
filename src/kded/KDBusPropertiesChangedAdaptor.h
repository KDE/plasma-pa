// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2020 Harald Sitter <sitter@kde.org>

#pragma once

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusMetaType>
#include <QMetaProperty>

// QtDBus doesn't implement PropertiesChanged for some reason.
// This is meant to be childed' on an object to track that
// objects' property notifications.
class KDBusPropertiesChangedAdaptor : public QObject
{
    Q_OBJECT
public:
    KDBusPropertiesChangedAdaptor(const QString &objectPath, QObject *adaptee)
        : QObject(adaptee)
        , m_objectPath(objectPath)
    {
        auto mo = adaptee->metaObject();
        for (int i = 0; i < mo->propertyCount(); ++i) {
            QMetaProperty property = mo->property(i);
            if (!property.hasNotifySignal()) {
                continue;
            }
            const int fooIndex = metaObject()->indexOfMethod("onPropertyChanged()"); // of adaptor
            Q_ASSERT(fooIndex != -1);
            connect(adaptee, property.notifySignal(), this, metaObject()->method(fooIndex));
        }
    }

private Q_SLOTS:
    void onPropertyChanged()
    {
        if (!sender() || senderSignalIndex() == -1) {
            return;
        }
        auto mo = sender()->metaObject();
        for (int i = 0; i < mo->propertyCount(); ++i) {
            QMetaProperty property = mo->property(i);
            if (!property.hasNotifySignal()) {
                continue;
            }
            if (property.notifySignalIndex() != senderSignalIndex()) {
                continue;
            }
            const int ciid = mo->indexOfClassInfo("D-Bus Interface");
            if (ciid == -1) {
                continue;
            }
            QDBusMessage signal = QDBusMessage::createSignal(m_objectPath, //
                                                             QStringLiteral("org.freedesktop.DBus.Properties"),
                                                             QStringLiteral("PropertiesChanged"));
            signal << QString::fromLatin1(mo->classInfo(ciid).value());
            signal << QVariantMap({{QString::fromLatin1(property.name()), property.read(sender())}}); // changed properties DICT<STRING,VARIANT>
            signal << QStringList(); // invalidated property names no clue what invalidation means
            QDBusConnection::sessionBus().send(signal);
        }
    }

private:
    const QString m_objectPath;
};
