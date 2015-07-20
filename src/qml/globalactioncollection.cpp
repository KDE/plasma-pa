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
    emit nameChanged();
}

QString GlobalActionCollection::displayName() const
{
    return m_displayName;
}

void GlobalActionCollection::setDisplayName(const QString &displayName)
{
    m_displayName = displayName;
    emit displayNameChanged();
}

void GlobalActionCollection::componentComplete()
{
    foreach (QObject *item, children()) {
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
