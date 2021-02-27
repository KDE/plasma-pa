/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "globalactioncollection.h"

#include <KGlobalAccel>

GlobalAction::GlobalAction(QObject *parent)
    : QAction(parent)
{
}

GlobalActionCollection::GlobalActionCollection()
{
}

QString GlobalActionCollection::name() const
{
    return m_name;
}

void GlobalActionCollection::setName(const QString &name)
{
    m_name = name;
    Q_EMIT nameChanged();
}

QString GlobalActionCollection::displayName() const
{
    return m_displayName;
}

void GlobalActionCollection::setDisplayName(const QString &displayName)
{
    m_displayName = displayName;
    Q_EMIT displayNameChanged();
}

void GlobalActionCollection::componentComplete()
{
    Q_FOREACH (QObject *item, children()) {
        GlobalAction *action = qobject_cast<GlobalAction *>(item);
        if (!action) {
            continue;
        }
        action->setProperty("componentName", m_name);
        action->setProperty("componentDisplayName", m_displayName);

        KGlobalAccel::setGlobalShortcut(action, action->shortcuts());
    }

    QQuickItem::componentComplete();
}
