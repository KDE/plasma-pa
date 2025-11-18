/*
    SPDX-FileCopyrightText: 2023 Bharadwaj Raju <bharadwaj.raju777@gmail.com>
    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "audioshortcutsservice.h"

#include "audioicon.h"

#include <optional>

#include <QAction>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QGuiApplication>
#include <QStringBuilder>

#include <KGlobalAccel>
#include <KLocalizedString>
#include <KPluginFactory>

#include <PulseAudioQt/Device>
#include <PulseAudioQt/Server>
#include <PulseAudioQt/Sink>
#include <PulseAudioQt/Source>

#include <QCoroDBusPendingCall>

#include "mutedmicrophonereminder.h"
#include "preferreddevice.h"

K_PLUGIN_CLASS_WITH_JSON(AudioShortcutsService, "audioshortcutsservice.json")

constexpr QLatin1String OSD_DBUS_SERVICE = "org.kde.plasmashell"_L1;
constexpr QLatin1String OSD_DBUS_PATH = "/org/kde/osdService"_L1;

AudioShortcutsService::AudioShortcutsService(QObject *parent, const QList<QVariant> &)
    : KDEDModule(parent)
    , m_sinkModel(new PulseAudioQt::SinkModel(this))
    , m_sourceModel(new PulseAudioQt::SourceModel(this))
    , m_cardModel(new PulseAudioQt::CardModel(this))
    , m_globalConfig(new GlobalConfig(this))
    , m_globalConfigWatcher(KConfigWatcher::create(m_globalConfig->sharedConfig()))
    , m_osdDBusInterface(new OsdServiceInterface(OSD_DBUS_SERVICE, OSD_DBUS_PATH, QDBusConnection::sessionBus(), this))
    , m_feedback(new VolumeFeedback(this))
{
    connect(m_globalConfigWatcher.get(), &KConfigWatcher::configChanged, this, [this](const KConfigGroup &group, const QByteArrayList &names) {
        if (group.name() == "General"_ba && names.contains("MutedMicrophoneReminderOsd"_ba)) {
            if (m_globalConfig->mutedMicrophoneReminderOsd()) {
                m_mutedMicrophoneReminder = std::make_unique<MutedMicrophoneReminder>();
            } else {
                m_mutedMicrophoneReminder.reset();
            }
        }
    });
    if (m_globalConfig->mutedMicrophoneReminderOsd()) {
        m_mutedMicrophoneReminder = std::make_unique<MutedMicrophoneReminder>();
    }

    connect(PulseAudioQt::Context::instance()->server(), &PulseAudioQt::Server::defaultSinkChanged, this, &AudioShortcutsService::handleDefaultSinkChange);
    connect(&m_preferredDevice, &PreferredDevice::sinkChanged, this, [this]() {
        auto sink = m_preferredDevice.sink();
        if (!sink) {
            return;
        }
        connect(sink, &PulseAudioQt::Sink::volumeChanged, this, [this]() {
            auto sink = m_preferredDevice.sink();
            if (!sink) {
                return;
            }
            showVolume(volumePercent(sink->volume()));
        });
    });

    // For global mute, we need to track sinks/sources and changes to them
    connect(m_sinkModel, &PulseAudioQt::SinkModel::rowsInserted, this, &AudioShortcutsService::handleNewSink);
    connect(m_sourceModel, &PulseAudioQt::SinkModel::rowsInserted, this, &AudioShortcutsService::handleNewSource);
    connect(m_sinkModel, &PulseAudioQt::SinkModel::dataChanged, this, &AudioShortcutsService::handleSinkDataChanged);
    connect(m_sourceModel, &PulseAudioQt::SourceModel::dataChanged, this, &AudioShortcutsService::handleSourceDataChanged);

    QList<QAction *> actions;

    QAction *volumeUpAction = new QAction(this);
    actions.append(volumeUpAction);
    volumeUpAction->setObjectName(u"increase_volume"_s);
    volumeUpAction->setText(i18n("Increase Volume"));
    volumeUpAction->setShortcut(Qt::Key_VolumeUp);
    connect(volumeUpAction, &QAction::triggered, this, [this]() {
        auto sink = m_preferredDevice.sink();
        if (!sink) {
            return;
        }
        int percent = changeVolumePercent(sink, m_globalConfig->volumeStep());
        showVolume(percent);
        playFeedback(-1);
    });

    QAction *volumeDownAction = new QAction(this);
    actions.append(volumeDownAction);
    volumeDownAction->setObjectName(u"decrease_volume"_s);
    volumeDownAction->setText(i18n("Decrease Volume"));
    volumeDownAction->setShortcut(Qt::Key_VolumeDown);
    connect(volumeDownAction, &QAction::triggered, this, [this]() {
        auto sink = m_preferredDevice.sink();
        if (!sink) {
            return;
        }
        int percent = changeVolumePercent(sink, -m_globalConfig->volumeStep());
        showVolume(percent);
        playFeedback(-1);
    });

    QAction *volumeUpSmallAction = new QAction(this);
    actions.append(volumeUpSmallAction);
    volumeUpSmallAction->setObjectName(u"increase_volume_small"_s);
    volumeUpSmallAction->setText(i18n("Increase Volume by 1%"));
    volumeUpSmallAction->setShortcut(Qt::ShiftModifier | Qt::Key_VolumeUp);
    connect(volumeUpSmallAction, &QAction::triggered, this, [this]() {
        auto sink = m_preferredDevice.sink();
        if (!sink) {
            return;
        }
        int percent = changeVolumePercent(sink, 1);
        showVolume(percent);
        playFeedback(-1);
    });

    QAction *volumeDownSmallAction = new QAction(this);
    actions.append(volumeDownSmallAction);
    volumeDownSmallAction->setObjectName(u"decrease_volume_small"_s);
    volumeDownSmallAction->setText(i18n("Decrease Volume by 1%"));
    volumeDownSmallAction->setShortcut(Qt::ShiftModifier | Qt::Key_VolumeDown);
    connect(volumeDownSmallAction, &QAction::triggered, this, [this]() {
        auto sink = m_preferredDevice.sink();
        if (!sink) {
            return;
        }
        int percent = changeVolumePercent(sink, -1);
        showVolume(percent);
        playFeedback(-1);
    });

    QAction *volumeUpMicAction = new QAction(this);
    actions.append(volumeUpMicAction);
    volumeUpMicAction->setObjectName(u"increase_microphone_volume"_s);
    volumeUpMicAction->setText(i18n("Increase Microphone Volume"));
    volumeUpMicAction->setShortcut(Qt::Key_MicVolumeUp);
    connect(volumeUpMicAction, &QAction::triggered, this, [this]() {
        auto defaultSource = PulseAudioQt::Context::instance()->server()->defaultSource();
        if (!defaultSource) {
            return;
        }
        int percent = changeVolumePercent(defaultSource, m_globalConfig->volumeStep());
        showMicVolume(percent);
    });

    QAction *volumeDownMicAction = new QAction(this);
    actions.append(volumeDownMicAction);
    volumeDownMicAction->setObjectName(u"decrease_microphone_volume"_s);
    volumeDownMicAction->setText(i18n("Decrease Microphone Volume"));
    volumeDownMicAction->setShortcut(Qt::Key_MicVolumeDown);
    connect(volumeDownMicAction, &QAction::triggered, this, [this]() {
        auto defaultSource = PulseAudioQt::Context::instance()->server()->defaultSource();
        if (!defaultSource) {
            return;
        }
        int percent = changeVolumePercent(defaultSource, -m_globalConfig->volumeStep());
        showMicVolume(percent);
    });

    QAction *muteAction = new QAction(this);
    actions.append(muteAction);
    muteAction->setObjectName(u"mute"_s);
    muteAction->setText(i18n("Mute"));
    muteAction->setShortcut(Qt::Key_VolumeMute);
    connect(muteAction, &QAction::triggered, this, [this]() {
        if (m_globalConfig->globalMuteSinks()) {
            disableGlobalSinkMute();
        } else {
            enableGlobalSinkMute();
        }
    });

    QAction *muteMicAction = new QAction(this);
    actions.append(muteMicAction);
    muteMicAction->setObjectName(u"mic_mute"_s);
    muteMicAction->setText(i18n("Mute Microphone"));
    muteMicAction->setShortcuts({Qt::Key_MicMute, Qt::MetaModifier | Qt::Key_VolumeMute});
    connect(muteMicAction, &QAction::triggered, this, [this]() {
        if (m_globalConfig->globalMuteSources()) {
            disableGlobalSourceMute();
        } else {
            enableGlobalSourceMute();
        }
    });

    for (const auto action : actions) {
        action->setProperty("componentName", u"kmix"_s);
        action->setProperty("componentDisplayName", i18n("Audio Volume"));
        KGlobalAccel::setGlobalShortcut(action, action->shortcuts());
    }
}

qint64 AudioShortcutsService::boundVolume(qint64 volume, int maxVolume)
{
    return qMax(PulseAudioQt::minimumVolume(), qMin(volume, maxVolume));
}

int AudioShortcutsService::volumePercent(qint64 volume)
{
    return std::round((double)volume / PulseAudioQt::normalVolume() * 100.0);
}

QString AudioShortcutsService::nameForDevice(const PulseAudioQt::Device *device)
{
    if (!device) {
        return i18n("No such device");
    }

    if (!device->description().isEmpty()) {
        return device->description();
    }

    if (!device->name().isEmpty()) {
        return device->name();
    }

    return i18n("Device name not found");
}

int AudioShortcutsService::changeVolumePercent(PulseAudioQt::Device *device, int deltaPercent)
{
    const qint64 oldVolume = device->volume();
    const int oldPercent = volumePercent(oldVolume);
    const int targetPercent = oldPercent + deltaPercent;
    const int maxVolume = PulseAudioQt::normalVolume() * (m_globalConfig->raiseMaximumVolume() ? 150 : 100) / 100.0;
    const qint64 newVolume = boundVolume(std::round(PulseAudioQt::normalVolume() * (targetPercent / 100.f)), maxVolume);
    const int newPercent = volumePercent(newVolume);
    device->setMuted(newPercent == 0);
    device->setVolume(newVolume);
    return newPercent;
}

QCoro::Task<> AudioShortcutsService::handleDefaultSinkChange()
{
    const PulseAudioQt::Sink *defaultSink = PulseAudioQt::Context::instance()->server()->defaultSink();

    // we don't want to show the OSD on startup nor when we didn't have a sink before (e.g. after a TTY change).
    const bool hadDefaultSink = m_hasDefaultSink;
    m_hasDefaultSink = (defaultSink != nullptr);

    if (!m_globalConfig->defaultOutputDeviceOsd()) {
        co_return;
    }

    if (!hadDefaultSink || !defaultSink) {
        co_return;
    }

    // Could be cone after co_await.
    QPointer<OsdServiceInterface> osd = m_osdDBusInterface;

    QString icon = AudioIcon::forFormFactor(defaultSink->formFactor());
    QString description = nameForDevice(defaultSink);
    std::optional<int> batteryPercentage;

    if (defaultSink->name() == DUMMY_OUTPUT_NAME) {
        description = i18n("No output device");
        if (icon.isEmpty()) {
            icon = QGuiApplication::layoutDirection() == Qt::RightToLeft ? u"audio-volume-muted-rtl"_s : u"audio-volume-muted"_s;
        }
    } else {
        if (icon.isEmpty()) {
            icon = AudioIcon::forVolume(volumePercent(defaultSink->volume()), defaultSink->isMuted(), QString());
        }

        auto cardIdx = m_cardModel->index(-1, 0);
        for (int i = 0; i < m_cardModel->rowCount(); i++) {
            const auto idx = m_cardModel->index(i, 0);
            if (m_cardModel->data(idx, m_cardModel->role("Index"_ba)) == defaultSink->cardIndex()) {
                cardIdx = idx;
            }
        }
        if (cardIdx.isValid()) {
            const QVariantMap cardProperties = m_cardModel->data(cardIdx, m_cardModel->role("Properties"_ba)).toMap();
            bool convOk = false;
            const int cardBluetoothBattery = cardProperties[u"bluetooth.battery"_s].toString().remove('%').toInt(&convOk);
            if (convOk) {
                batteryPercentage = cardBluetoothBattery;
            } else {
                // Ask Bluez, if it is a bluetooth device.
                const QString bluezPath = cardProperties[u"api.bluez5.path"_s].toString();
                if (!bluezPath.isEmpty()) {
                    QDBusMessage batteryMessage = QDBusMessage::createMethodCall(u"org.bluez"_s, bluezPath, u"org.freedesktop.DBus.Properties"_s, u"Get"_s);
                    batteryMessage.setArguments({u"org.bluez.Battery1"_s, u"Percentage"_s});

                    QDBusReply<QDBusVariant> reply = co_await QDBusConnection::sessionBus().asyncCall(batteryMessage);
                    if (reply.isValid()) {
                        bool ok;
                        // NOTE "Percentage" on org.bluez.Battery1 is of type Byte (y).
                        const int percentage = reply.value().variant().toInt(&ok);
                        if (ok) {
                            batteryPercentage = percentage;
                        }
                    }
                }
            }
        }
    }

    if (batteryPercentage.has_value()) {
        description = i18nc("Device name (Battery percent)", "%1 (%2% Battery)", description, batteryPercentage.value());
    }

    if (osd) {
        osd->showText(icon, description);
    }
}

void AudioShortcutsService::muteVolume()
{
    auto sink = m_preferredDevice.sink();
    if (!sink || sink->name() == DUMMY_OUTPUT_NAME) {
        return;
    }
    if (!sink->isMuted()) {
        enableGlobalSinkMute();
        showMute(0);
    } else {
        if (m_globalConfig->globalMuteSinks()) {
            disableGlobalSinkMute();
        }
        sink->setMuted(false);
        showMute(sink->volume());
        playFeedback(-1);
    }
}

void AudioShortcutsService::playFeedback(int sinkIdx)
{
    if (!m_globalConfig->audioFeedback()) {
        return;
    }
    auto sink = m_preferredDevice.sink();
    if (sinkIdx == -1 && sink) {
        sinkIdx = sink->index();
    }
    m_feedback->play(sinkIdx);
}

void AudioShortcutsService::showMute(int percent)
{
    if (!m_globalConfig->muteOsd()) {
        return;
    }
    m_osdDBusInterface->volumeChanged(percent, m_globalConfig->raiseMaximumVolume() ? 150 : 100);
}

void AudioShortcutsService::showVolume(int percent)
{
    if (!m_globalConfig->volumeOsd()) {
        return;
    }
    m_osdDBusInterface->volumeChanged(percent, m_globalConfig->raiseMaximumVolume() ? 150 : 100);
}

void AudioShortcutsService::showMicVolume(int percent)
{
    if (!m_globalConfig->microphoneSensitivityOsd()) {
        return;
    }
    m_osdDBusInterface->microphoneVolumeChanged(percent);
}

void AudioShortcutsService::showMicMute(int percent)
{
    if (!m_globalConfig->muteOsd()) {
        return;
    }
    m_osdDBusInterface->microphoneVolumeChanged(percent);
}

void AudioShortcutsService::applyGlobalSinkMute(const QModelIndex &index, const bool globalMute, QStringList &globalMuteMutedDevices)
{
    const QString name = m_sinkModel->data(index, m_sinkModel->role("Name"_ba)).toString();
    const QString activePortIdx = QString::number(m_sinkModel->data(index, m_sinkModel->role("ActivePortIndex"_ba)).toUInt());
    const QString deviceId = name % "." % activePortIdx;

    if (globalMute) {
        // Mute the device if unmuted
        const bool isUnmuted = !m_sinkModel->data(index, m_sinkModel->role("Muted"_ba)).toBool();
        if (isUnmuted) {
            if (!globalMuteMutedDevices.contains(deviceId)) {
                globalMuteMutedDevices.append(deviceId);
            }
            m_sinkModel->setData(index, true, m_sinkModel->role("Muted"_ba));
        }
    } else {
        // Unmute the device if global mute has muted it
        if (globalMuteMutedDevices.removeAll(deviceId)) {
            m_sinkModel->setData(index, false, m_sinkModel->role("Muted"_ba));
        }
    }
}

void AudioShortcutsService::applyGlobalSourceMute(const QModelIndex &index, const bool globalMute, QStringList &globalMicMuteMutedDevices)
{
    const QString name = m_sourceModel->data(index, m_sourceModel->role("Name"_ba)).toString();
    const QString activePortIdx = QString::number(m_sourceModel->data(index, m_sourceModel->role("ActivePortIndex"_ba)).toUInt());
    const QString deviceId = name % "." % activePortIdx;

    if (globalMute) {
        // Mute the device if unmuted
        const bool isUnmuted = !m_sourceModel->data(index, m_sourceModel->role("Muted"_ba)).toBool();
        if (isUnmuted) {
            if (!globalMicMuteMutedDevices.contains(deviceId)) {
                globalMicMuteMutedDevices.append(deviceId);
            }
            m_sourceModel->setData(index, true, m_sourceModel->role("Muted"_ba));
        }
    } else {
        // Unmute the device if global mute has muted it
        if (globalMicMuteMutedDevices.removeAll(deviceId)) {
            m_sourceModel->setData(index, false, m_sourceModel->role("Muted"_ba));
        }
    }
}

void AudioShortcutsService::handleNewSink(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)

    QStringList globalMuteMutedDevices = m_globalConfig->globalMuteSinksMutedDevices();
    const bool globalMute = m_globalConfig->globalMuteSinks();

    for (int i = first; i <= last; ++i) {
        applyGlobalSinkMute(m_sinkModel->index(i, 0), globalMute, globalMuteMutedDevices);
    }

    m_globalConfig->setGlobalMuteSinksMutedDevices(globalMuteMutedDevices);
    m_globalConfig->save();
}

void AudioShortcutsService::handleNewSource(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)

    QStringList globalMicMuteMutedDevices = m_globalConfig->globalMuteSourcesMutedDevices();
    const bool globalMicMute = m_globalConfig->globalMuteSources();

    for (int i = first; i <= last; ++i) {
        applyGlobalSourceMute(m_sinkModel->index(i, 0), globalMicMute, globalMicMuteMutedDevices);
    }

    m_globalConfig->setGlobalMuteSourcesMutedDevices(globalMicMuteMutedDevices);
    m_globalConfig->save();
}

void AudioShortcutsService::handleSinkDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QList<int> &roles)
{
    QStringList globalMuteMutedDevices = m_globalConfig->globalMuteSinksMutedDevices();
    const bool globalMute = m_globalConfig->globalMuteSinks();

    for (int i = topLeft.row(); i <= bottomRight.row(); ++i) {
        const auto index = m_sinkModel->index(i, 0);

        const bool activePortIndexChanged = roles.contains(m_sinkModel->role("ActivePortIndex"_ba));
        const bool volumeChanged = roles.contains(m_sinkModel->role("Volume"_ba));
        const bool mutedChanged = roles.contains(m_sinkModel->role("Muted"_ba));
        const bool isUnmuted = !m_sinkModel->data(index, m_sinkModel->role("Muted"_ba)).toBool();

        if (activePortIndexChanged) {
            // Apply global mute to new device
            applyGlobalSinkMute(index, globalMute, globalMuteMutedDevices);

            m_globalConfig->setGlobalMuteSinksMutedDevices(globalMuteMutedDevices);
            m_globalConfig->save();
        } else if (globalMute && (volumeChanged || (mutedChanged && isUnmuted))) {
            // Changes to volume or unmuting break global mute
            disableGlobalSinkMute();
        }
    }
}

void AudioShortcutsService::handleSourceDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QList<int> &roles)
{
    QStringList globalMicMuteMutedDevices = m_globalConfig->globalMuteSourcesMutedDevices();
    const bool globalMicMute = m_globalConfig->globalMuteSources();

    for (int i = topLeft.row(); i <= bottomRight.row(); ++i) {
        const auto index = m_sourceModel->index(i, 0);

        const bool activePortIndexChanged = roles.contains(m_sourceModel->role("ActivePortIndex"_ba));
        const bool volumeChanged = roles.contains(m_sourceModel->role("Volume"_ba));
        const bool mutedChanged = roles.contains(m_sourceModel->role("Muted"_ba));
        const bool isUnmuted = !m_sourceModel->data(index, m_sourceModel->role("Muted"_ba)).toBool();

        if (activePortIndexChanged) {
            // Apply global mute to new device
            applyGlobalSourceMute(index, globalMicMute, globalMicMuteMutedDevices);

            m_globalConfig->setGlobalMuteSourcesMutedDevices(globalMicMuteMutedDevices);
            m_globalConfig->save();
        } else if (globalMicMute && (volumeChanged || (mutedChanged && isUnmuted))) {
            // Changes to volume or unmuting break global mute
            disableGlobalSourceMute();
        }
    }
}

void AudioShortcutsService::enableGlobalSinkMute()
{
    QStringList globalMuteMutedDevices = m_globalConfig->globalMuteSinksMutedDevices();
    for (int i = 0; i < m_sinkModel->rowCount(); ++i) {
        applyGlobalSinkMute(m_sinkModel->index(i, 0), true, globalMuteMutedDevices);
    }

    m_globalConfig->setGlobalMuteSinksMutedDevices(globalMuteMutedDevices);
    m_globalConfig->setGlobalMuteSinks(true);
    m_globalConfig->save();

    showMute(0);
}

void AudioShortcutsService::enableGlobalSourceMute()
{
    QStringList globalMicMuteMutedDevices = m_globalConfig->globalMuteSourcesMutedDevices();
    for (int i = 0; i < m_sourceModel->rowCount(); ++i) {
        applyGlobalSourceMute(m_sourceModel->index(i, 0), true, globalMicMuteMutedDevices);
    }

    m_globalConfig->setGlobalMuteSourcesMutedDevices(globalMicMuteMutedDevices);
    m_globalConfig->setGlobalMuteSources(true);
    m_globalConfig->save();

    showMicMute(0);
}

void AudioShortcutsService::disableGlobalSinkMute()
{
    QStringList globalMuteMutedDevices = m_globalConfig->globalMuteSinksMutedDevices();
    for (int i = 0; i < m_sinkModel->rowCount(); ++i) {
        applyGlobalSinkMute(m_sinkModel->index(i, 0), false, globalMuteMutedDevices);
    }

    m_globalConfig->setGlobalMuteSinksMutedDevices(globalMuteMutedDevices);
    m_globalConfig->setGlobalMuteSinks(false);
    m_globalConfig->save();

    const auto sink = m_preferredDevice.sink();
    if (sink) {
        showMute(volumePercent(sink->volume()));
        playFeedback(-1);
    }
}

void AudioShortcutsService::disableGlobalSourceMute()
{
    QStringList globalMicMuteMutedDevices = m_globalConfig->globalMuteSourcesMutedDevices();
    for (int i = 0; i < m_sourceModel->rowCount(); ++i) {
        applyGlobalSourceMute(m_sourceModel->index(i, 0), false, globalMicMuteMutedDevices);
    }

    m_globalConfig->setGlobalMuteSourcesMutedDevices(globalMicMuteMutedDevices);
    m_globalConfig->setGlobalMuteSources(false);
    m_globalConfig->save();

    const auto source = m_preferredDevice.source();
    if (source)
        showMicMute(volumePercent(source->volume()));
}

#include "audioshortcutsservice.moc"

#include "moc_audioshortcutsservice.cpp"
