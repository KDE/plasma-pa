/*
    SPDX-FileCopyrightText: 2016 David Edmundson <davidedmundson@kde.org>
    SPDX-FileCopyrightText: 2024 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "modulemanager.h"

#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

#include <PulseAudioQt/Context>
#include <PulseAudioQt/Module>

using namespace Qt::StringLiterals;

namespace
{
inline QString configDir()
{
    return QDir::homePath() + QLatin1String("/.config/pipewire/pipewire-pulse.conf.d/");
}

void loadModuleByName(const QString &moduleName)
{
    PulseAudioQt::Context::instance()->loadModule(moduleName, QString());
}

void unloadModuleByName(const QString &moduleName)
{
    const auto modules = PulseAudioQt::Context::instance()->modules();
    for (const auto &module : modules) {
        if (module->name() == moduleName) {
            PulseAudioQt::Context::instance()->unloadModule(module);
        }
    }
}
} // namespace

namespace PulseAudioQt
{
class GenericModule
{
    using MakeConfigurationCallback = std::function<QJsonObject(const QString &moduleName)>;
    using DisableModuleCallback = std::function<QJsonObject(const QString &moduleName)>;

public:
    GenericModule(const QString &moduleName);
    virtual ~GenericModule() = default;
    Q_DISABLE_COPY_MOVE(GenericModule)

    [[nodiscard]] bool isEnabled() const;
    void setEnabled(bool enabled);

    // Called to create the QJsonObject configuration for this module
    [[nodiscard]] virtual QJsonObject makeConfiguration();
    // Called to enable the module (i.e. load it server-side)
    virtual void loadModule();
    // Called to disable the module (i.e. unload it server-side)
    virtual void unloadModule();

protected:
    QString m_moduleName;
    QString m_configName = u"00-plasma-pa-%1.conf"_s.arg(m_moduleName);
    QString m_configFile = configDir() + m_configName;
};

GenericModule::GenericModule(const QString &moduleName)
    : m_moduleName(moduleName)
{
}

QJsonObject GenericModule::makeConfiguration()
{
    QJsonObject loadModuleCommand({
        {"cmd"_L1, "load-module"},
        {"args"_L1, m_moduleName},
        {"flags"_L1, QJsonArray()},
    });
    return QJsonObject({{"pulse.cmd"_L1, QJsonArray({loadModuleCommand})}});
}

void GenericModule::loadModule()
{
    loadModuleByName(m_moduleName);
}

void GenericModule::unloadModule()
{
    unloadModuleByName(m_moduleName);
}

bool GenericModule::isEnabled() const
{
    return QFile::exists(m_configFile);
}

void GenericModule::setEnabled(bool enabled)
{
    if (enabled) {
        if (!QDir().mkpath(configDir())) {
            qWarning("Failed to create config directory %s!", qPrintable(configDir()));
            return;
        }
        QFile config(m_configFile);
        if (!config.open(QIODevice::WriteOnly)) {
            qWarning("Failed to open config file %s!", qPrintable(m_configFile));
            return;
        }
        if (config.write(QJsonDocument(makeConfiguration()).toJson()) == -1) {
            qWarning("Failed to write to config file %s!", qPrintable(m_configFile));
            return;
        }
        loadModule();
    } else {
        unloadModule();
        if (QFile::exists(m_configFile) && !QFile::remove(m_configFile)) {
            qWarning("Failed to remove config file %s!", qPrintable(m_configFile));
            return;
        }
    }
}

// SwitchOnConnectModule is a special case of GenericModule that also disables the device-manager module.
// device-manager and switch-on-connect try to do similar things and get in each other's way.
class SwitchOnConnectModule : public GenericModule
{
public:
    using GenericModule::GenericModule;

    [[nodiscard]] QJsonObject makeConfiguration() override
    {
        auto config = GenericModule::makeConfiguration();
        // Additionally disable device-manager. This is a builtin option of pipewire-pulse.conf.
        config.insert("pulse.properties"_L1, QJsonObject({{"pulse.cmd.device-manager"_L1, false}}));
        return config;
    }

    void loadModule() override
    {
        unloadModuleByName("module-device-manager"_L1);
        loadModuleByName(m_moduleName);
    }

    void unloadModule() override
    {
        unloadModuleByName(m_moduleName);
        loadModuleByName("module-device-manager"_L1);
    }
};

ModuleManager::ModuleManager(QObject *parent)
    : QObject(parent)
    , m_combineSinks(new GenericModule(u"module-combine-sink"_s))
    , m_switchOnConnect(new SwitchOnConnectModule(u"module-switch-on-connect"_s))
{
}

ModuleManager::~ModuleManager() = default;

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
    return m_switchOnConnect->isEnabled();
}

void ModuleManager::setSwitchOnConnect(bool switchOnConnect)
{
    m_switchOnConnect->setEnabled(switchOnConnect);
}

} // namespace PulseAudioQt

#include "moc_modulemanager.cpp"
