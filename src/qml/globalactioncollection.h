/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef GLOBALACTIONCOLLECTION_H
#define GLOBALACTIONCOLLECTION_H

#include <QAction>
#include <QQuickItem>

class GlobalAction : public QAction
{
    Q_OBJECT
    // These members exist in QAction but only "shortcut" is exposed to the metaobject
    Q_PROPERTY(QList<QKeySequence> shortcuts READ shortcuts WRITE setShortcuts NOTIFY changed)
public:
    explicit GlobalAction(QObject *parent = nullptr);
};

class GlobalActionCollection : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString displayName READ displayName WRITE setDisplayName NOTIFY displayNameChanged)
public:
    GlobalActionCollection();

    QString name() const;
    void setName(const QString &name);

    QString displayName() const;
    void setDisplayName(const QString &displayName);

Q_SIGNALS:
    void nameChanged();
    void displayNameChanged();

protected:
    void componentComplete() final;

private:
    QString m_name;
    QString m_displayName;
};

#endif // GLOBALACTIONCOLLECTION_H
