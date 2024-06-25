// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Harald Sitter <sitter@kde.org>

#include "devicerenamesaver.h"

#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QProcess>

#include <PulseAudioQt/Context>

#include <KLocalizedString>

#include "debug.h"

using namespace std::chrono_literals;
using namespace Qt::StringLiterals;

namespace
{
constexpr auto MONITOR_ALSA_RULES = "monitor.alsa.rules"_L1;
constexpr auto MONITOR_BLUEZ_RULES = "monitor.bluez.rules"_L1;
constexpr auto MONITOR_BLUEZ_MIDI_RULES = "monitor.bluez-midi.rules"_L1;
constexpr auto MONITOR_RULES = {MONITOR_ALSA_RULES, MONITOR_BLUEZ_RULES, MONITOR_BLUEZ_MIDI_RULES};

constexpr auto ALSA_PREFIX = "alsa_"_L1;
constexpr auto BLUEZ_PREFIX = "bluez_"_L1;
constexpr auto BLUEZ_MIDI_PREFIX = "bluez_midi_"_L1;

inline QString configDir()
{
    return QDir::homePath() + QLatin1String("/.config/wireplumber/wireplumber.conf.d");
}

inline QString configPath()
{
    return configDir() + QLatin1String("/00-plasma-pa.conf");
}
} // namespace

DeviceRenameSaver::DeviceRenameSaver(QObject *parent)
    : QObject(parent)
{
    // This is a bit garbage but the best we can do.
    // We don't know when the reload is actually complete (as in: all devices have been re-enumerated). So we use
    // data changes as indication of busyness.
    // The delay is arbitrary long to be relatively certain that the devices have settled.
    m_readyTimer.setInterval(250ms);
    m_readyTimer.setSingleShot(true);
    connect(&m_readyTimer, &QTimer::timeout, this, [this] {
        if (!m_busy) {
            return;
        }

        if (PulseAudioQt::Context::instance()->state() != PulseAudioQt::Context::State::Ready) {
            return;
        }

        m_readyTimer.stop();
        m_busy = false;
        Q_EMIT busyChanged();
    });

    auto context = PulseAudioQt::Context::instance();
    connect(context, &PulseAudioQt::Context::stateChanged, &m_readyTimer, qOverload<>(&QTimer::start));
    connect(context, &PulseAudioQt::Context::sinkAdded, &m_readyTimer, qOverload<>(&QTimer::start));
    connect(context, &PulseAudioQt::Context::sinkRemoved, &m_readyTimer, qOverload<>(&QTimer::start));
    connect(context, &PulseAudioQt::Context::sinkInputAdded, &m_readyTimer, qOverload<>(&QTimer::start));
    connect(context, &PulseAudioQt::Context::sinkInputRemoved, &m_readyTimer, qOverload<>(&QTimer::start));
    connect(context, &PulseAudioQt::Context::sourceAdded, &m_readyTimer, qOverload<>(&QTimer::start));
    connect(context, &PulseAudioQt::Context::sourceRemoved, &m_readyTimer, qOverload<>(&QTimer::start));
    connect(context, &PulseAudioQt::Context::sourceOutputAdded, &m_readyTimer, qOverload<>(&QTimer::start));
    connect(context, &PulseAudioQt::Context::sourceOutputRemoved, &m_readyTimer, qOverload<>(&QTimer::start));
    connect(context, &PulseAudioQt::Context::clientAdded, &m_readyTimer, qOverload<>(&QTimer::start));
    connect(context, &PulseAudioQt::Context::clientRemoved, &m_readyTimer, qOverload<>(&QTimer::start));
    connect(context, &PulseAudioQt::Context::cardAdded, &m_readyTimer, qOverload<>(&QTimer::start));
    connect(context, &PulseAudioQt::Context::cardRemoved, &m_readyTimer, qOverload<>(&QTimer::start));
    connect(context, &PulseAudioQt::Context::moduleAdded, &m_readyTimer, qOverload<>(&QTimer::start));
    connect(context, &PulseAudioQt::Context::moduleRemoved, &m_readyTimer, qOverload<>(&QTimer::start));
    connect(context, &PulseAudioQt::Context::streamRestoreAdded, &m_readyTimer, qOverload<>(&QTimer::start));
    connect(context, &PulseAudioQt::Context::streamRestoreRemoved, &m_readyTimer, qOverload<>(&QTimer::start));
}

void DeviceRenameSaver::saveChanges()
{
    if (writeOverrides(m_overrides)) {
        restartWirePlumber();
        m_originalOverrides = m_overrides;
        Q_EMIT dirtyChanged();
    }
}

void DeviceRenameSaver::restartWirePlumber()
{
    auto systemctl = new QProcess(this);
    systemctl->setProgram(u"systemctl"_s);
    // NOTE: We also restart pipewire even though it is technically not necessary. The problem is that we need
    // information on our context `State` but that is not provided when only wireplumber restarts (i.e. the
    // pulseaudio socket remains working it just has no devices). To work around this problem we also
    // restart pipewire to forcefully reconnect to the daemon and get state change signals.
    systemctl->setArguments({u"--user"_s, u"restart"_s, u"wireplumber.service"_s, u"pipewire.service"_s});
    connect(systemctl, &QProcess::finished, this, [this, systemctl](int exitCode, QProcess::ExitStatus exitStatus) {
        systemctl->deleteLater();

        // Only here as a safeguard. It will also be started by changes on the pulsaudio context.
        m_readyTimer.start();

        switch (exitStatus) {
        case QProcess::CrashExit:
            qCWarning(PLASMAPA) << "Failed to restart wireplumber.service. systemctl crashed!";
            setError(xi18nc("@info:status error message",
                            "Changes have not been applied.<nl/>"
                            "Failed to restart wireplumber.service. The command crashed."));
            return;
        case QProcess::NormalExit:
            break;
        }

        if (exitCode != 0) {
            qCWarning(PLASMAPA) << "Failed to restart wireplumber.service. Unexpected exit code" << exitCode;
            setError(xi18nc("@info:status error message %1 is an integer exit code",
                            "Changes have not been applied.<nl/>"
                            "Failed to restart wireplumber.service. The command terminated with code: %1.",
                            QString::number(exitCode)));
            return;
        }
    });

    m_busy = true;
    Q_EMIT busyChanged();

    systemctl->start();
}

QHash<QString, QVariantMap> DeviceRenameSaver::readOverrides()
{
    // SPA-JSON is a superset of JSON so we can simply treat things as JSON and we'll be fine.
    // We write JSON we read JSON.

    QFile file(configPath());
    if (!file.open(QFile::ReadOnly)) {
        qWarning() << "Failed to open file for reading" << file.fileName() << file.errorString();
        return {};
    }

    QJsonParseError error;
    const auto document = QJsonDocument::fromJson(file.readAll(), &error);
    if (error.error != QJsonParseError::NoError) {
        qWarning() << "Failed to parse config file" << error.error << error.errorString();
        return {};
    }

    QHash<QString, QVariantMap> data;
    const auto configObject = document.object();
    for (const auto &type : MONITOR_RULES) {
        const auto rulesArray = configObject.value(type).toArray();

        for (const auto &ruleObject : rulesArray) {
            const auto matchesArray = ruleObject["matches"_L1].toArray();
            const auto actionsObject = ruleObject["actions"_L1].toObject();

            Q_ASSERT(matchesArray.size() == 1);
            if (matchesArray.size() != 1) {
                qWarning() << "Invalid matches section" << ruleObject;
                continue;
            }
            Q_ASSERT(actionsObject.contains("update-props"_L1));
            if (!actionsObject.contains("update-props"_L1)) {
                qWarning() << "Invalid update-props section" << ruleObject;
                continue;
            }

            const auto deviceName = matchesArray.at(0).toObject()["node.name"_L1].toString();
            const auto properties = actionsObject["update-props"_L1].toObject().toVariantMap();

            Q_ASSERT(!deviceName.isEmpty());
            Q_ASSERT(!properties.isEmpty());
            if (deviceName.isEmpty() || properties.isEmpty()) {
                qWarning() << "Failed to parse rule." << deviceName.isEmpty() << properties.isEmpty() << ruleObject;
                continue;
            }

            data.insert(deviceName, properties);
        }
    }

    return data;
}

bool DeviceRenameSaver::writeOverrides(const QHash<QString, QVariantMap> &overrides)
{
    QList<QString> acceptedOverrides;

    const auto makeRules = [&acceptedOverrides, &overrides](const auto &accepts) {
        QJsonArray rules;
        for (const auto &[name, override] : overrides.asKeyValueRange()) {
            if (accepts(name)) {
                QJsonObject rule;
                rule.insert("matches"_L1,
                            QJsonArray({QJsonObject{{"node.name"_L1, name}, //
                                                    {"port.monitor"_L1, "!true"_L1}}}));
                rule.insert("actions"_L1, QJsonObject{{"update-props", QJsonObject::fromVariantMap(override)}});
                rules.append(rule);
                acceptedOverrides.append(name);
            }
        }
        return rules;
    };

    const auto alsaRules = makeRules([](const auto &name) {
        return name.startsWith(ALSA_PREFIX);
    });

    const auto bluezRules = makeRules([](const auto &name) {
        return name.startsWith(BLUEZ_PREFIX) && !name.startsWith(BLUEZ_MIDI_PREFIX);
    });

    const auto bluezMidiRules = makeRules([](const auto &name) {
        return name.startsWith(BLUEZ_MIDI_PREFIX);
    });

    if (acceptedOverrides.size() != overrides.size()) {
        const auto &accepted = acceptedOverrides;
        const auto &expected = overrides.keys();

        QStringList unaccepted;
        for (const auto &name : expected) {
            if (!accepted.contains(name)) {
                unaccepted.append(name);
            }
        }

        qCWarning(PLASMAPA) << "Unmapped devices!" << unaccepted << "context" << acceptedOverrides << "versus" << overrides.keys();
        setError(i18nc("@info error %1 is a list of device identifiers", "Failed to handle devices: %1.", unaccepted.join(','_L1)));
        return false;
    }

    QJsonDocument document{QJsonObject{
        {MONITOR_ALSA_RULES, alsaRules},
        {MONITOR_BLUEZ_RULES, bluezRules},
        {MONITOR_BLUEZ_MIDI_RULES, bluezMidiRules},
    }};

    if (!QDir(configDir()).exists()) {
        if (!QDir().mkpath(configDir())) {
            qCWarning(PLASMAPA) << "Failed to create" << configDir();
            setError(i18nc("@info error %1 is a path", "Failed to create directory: %1.", configDir()));
            return false;
        }
    }

    QFile file(configPath());
    if (!file.open(QFile::WriteOnly | QFile::Truncate)) {
        qCWarning(PLASMAPA) << "Failed to open file for writing" << file.fileName();
        setError(i18nc("@info error %1 is a path", "Failed to open file for writing: %1.", file.fileName()));
        return false;
    }
    const auto blob = document.toJson();
    const auto written = file.write(blob);
    Q_ASSERT(written == blob.size());
    return true;
}

bool DeviceRenameSaver::containsOverride(const QString &name) const
{
    return m_overrides.contains(name);
}

QVariantMap DeviceRenameSaver::override(const QString &name) const
{
    return m_overrides.value(name);
}

void DeviceRenameSaver::insertOverride(const QString &name, const QVariantMap &override)
{
    m_overrides.insert(name, override);
    Q_EMIT dirtyChanged();
}

void DeviceRenameSaver::removeOverride(const QString &name)
{
    m_overrides.remove(name);
    Q_EMIT dirtyChanged();
}

void DeviceRenameSaver::setModels(const QList<DeviceRenameModel *> &models)
{
    if (models == m_models) {
        return;
    }

    for (const auto &model : std::as_const(m_models)) {
        model->setSaver(nullptr);
    }

    m_models = models;

    for (const auto &model : std::as_const(m_models)) {
        model->setSaver(this);
    }
}

bool DeviceRenameSaver::isDirty() const
{
    return m_originalOverrides != m_overrides;
}

void DeviceRenameSaver::setError(const QString &error)
{
    m_error = error;
    Q_EMIT errorChanged();
}

bool DeviceRenameSaver::containsOriginalOverride(const QString &name) const
{
    return m_originalOverrides.contains(name);
}
