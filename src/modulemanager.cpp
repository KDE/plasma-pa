/*
    Copyright 2016 David Edmundson <davidedmundson@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "modulemanager.h"

#define PA_GCONF_ROOT "/system/pulseaudio"
#define PA_GCONF_PATH_MODULES PA_GCONF_ROOT"/modules"

#include "gconfitem.h"

namespace QPulseAudio
{
ModuleManager::ModuleManager(QObject *parent) :
    QObject(parent),
    m_combineSinksConfig(new GConfItem(QStringLiteral(PA_GCONF_PATH_MODULES"/combine"), this)),
    m_switchOnConnectConfig(new GConfItem(QStringLiteral(PA_GCONF_PATH_MODULES"/switch-on-connect"), this))
{
    connect(m_combineSinksConfig, &GConfItem::subtreeChanged, this, &ModuleManager::combineSinksChanged);
    connect(m_switchOnConnectConfig, &GConfItem::subtreeChanged, this, &ModuleManager::switchOnConnectChanged);
}

ModuleManager::~ModuleManager()
{
};


bool ModuleManager::combineSinks() const
{
    return m_combineSinksConfig->value(QStringLiteral("enabled")).toBool();
}

void ModuleManager::setCombineSinks(bool combineSinks)
{
    m_combineSinksConfig->set(QStringLiteral("locked"), true);

    if (combineSinks) {
        m_combineSinksConfig->set(QStringLiteral("name0"), QStringLiteral("module-combine"));
        m_combineSinksConfig->set(QStringLiteral("args0"), QVariant());

        m_combineSinksConfig->set(QStringLiteral("enabled"), true);
    } else {
        m_combineSinksConfig->set(QStringLiteral("enabled"), false);
    }
    m_combineSinksConfig->set(QStringLiteral("locked"), false);
}

bool ModuleManager::switchOnConnect() const
{
    return m_switchOnConnectConfig->value(QStringLiteral("enabled")).toBool();
}

void ModuleManager::setSwitchOnConnect(bool switchOnConnect)
{
    m_switchOnConnectConfig->set(QStringLiteral("locked"), true);

    if (switchOnConnect) {
        m_switchOnConnectConfig->set(QStringLiteral("name0"), QStringLiteral("module-switch-on-connect"));
        m_switchOnConnectConfig->set(QStringLiteral("args0"), QVariant());
        m_switchOnConnectConfig->set(QStringLiteral("enabled"), true);
    } else {
        m_switchOnConnectConfig->set(QStringLiteral("enabled"), false);
    }
    m_switchOnConnectConfig->set(QStringLiteral("locked"), false);
}

}
