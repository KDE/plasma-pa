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

class Module : public GConfItem
{
    Q_OBJECT
public:
    Module(const QString &configName, const QString &moduleName, QObject *parent);
    bool isEnabled() const;
    void setEnabled(bool enabled, const QVariant &args=QVariant());
private:
    QString m_moduleName;
};

Module::Module(const QString &configName, const QString &moduleName, QObject *parent) :
    GConfItem(QStringLiteral(PA_GCONF_PATH_MODULES"/") + configName, parent),
    m_moduleName(moduleName)
{
}

bool Module::isEnabled() const
{
    return value(QStringLiteral("enabled")).toBool();
}

void Module::setEnabled(bool enabled, const QVariant &args)
{
    set(QStringLiteral("locked"), true);

    if (enabled) {
        set(QStringLiteral("name0"), m_moduleName);
        set(QStringLiteral("args0"), args);
        set(QStringLiteral("enabled"), true);
    } else {
        set(QStringLiteral("enabled"), false);
    }
    set(QStringLiteral("locked"), false);
}


ModuleManager::ModuleManager(QObject *parent) :
    QObject(parent),
    m_combineSinks(new Module(QStringLiteral("combine"), QStringLiteral("module-combine"), this)),
    m_switchOnConnect(new Module(QStringLiteral("switch-on-connect"), QStringLiteral("module-switch-on-connect"), this)),
    m_deviceManager(new Module(QStringLiteral("device-manager"), QStringLiteral("module-device-manager"), this))
{
    connect(m_combineSinks, &GConfItem::subtreeChanged, this, &ModuleManager::combineSinksChanged);
    connect(m_switchOnConnect, &GConfItem::subtreeChanged, this, &ModuleManager::switchOnConnectChanged);
    connect(m_deviceManager, &GConfItem::subtreeChanged, this, &ModuleManager::switchOnConnectChanged);
}

ModuleManager::~ModuleManager()
{
};

bool ModuleManager::combineSinks() const
{
    return m_combineSinks->isEnabled();
}

void ModuleManager::setCombineSinks(bool combineSinks)
{
    m_combineSinks->setEnabled(combineSinks);
}

bool ModuleManager::switchOnConnect() const
{
    //switch on connect and device-manager do the same task. Only one should be enabled

    //Note on the first run m_deviceManager will appear to be disabled even though it's actually running
    //because there is no gconf entry, however m_switchOnConnect will only exist if set by Plasma PA
    //hence only check this entry
    return m_switchOnConnect->isEnabled() ;
}

void ModuleManager::setSwitchOnConnect(bool switchOnConnect)
{
    m_deviceManager->setEnabled(!switchOnConnect);
    m_switchOnConnect->setEnabled(switchOnConnect);
}

}

#include "modulemanager.moc"
