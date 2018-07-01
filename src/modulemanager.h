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

#ifndef MODULEMANAGER_H
#define MODULEMANAGER_H

#include <QString>

#include <pulse/introspect.h>

#include "context.h"
// Properties need fully qualified classes even with pointers.
#include "client.h"

class GConfItem;

namespace QPulseAudio
{
class GConfModule;

class ModuleManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool combineSinks READ combineSinks WRITE setCombineSinks NOTIFY combineSinksChanged)
    Q_PROPERTY(bool switchOnConnect READ switchOnConnect WRITE setSwitchOnConnect NOTIFY switchOnConnectChanged)
    Q_PROPERTY(QStringList loadedModules READ loadedModules NOTIFY loadedModulesChanged)
public:
    explicit ModuleManager(QObject *parent = nullptr);
    ~ModuleManager() override;
    bool combineSinks() const;
    void setCombineSinks(bool combineSinks);
    bool switchOnConnect() const;
    void setSwitchOnConnect(bool switchOnConnect);
    QStringList loadedModules() const;

Q_SIGNALS:
    void combineSinksChanged();
    void switchOnConnectChanged();
    void loadedModulesChanged();

private:
    void updateLoadedModules();

    GConfModule *m_combineSinks;
    GConfModule *m_switchOnConnect;
    GConfModule *m_deviceManager;
    QStringList m_loadedModules;
};

} // QPulseAudio

#endif // STREAM_H
