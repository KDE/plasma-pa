/*
    SPDX-FileCopyrightText: 2022 Bharadwaj Raju <bharadwaj.raju777@protonmail.com>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami
import org.kde.plasma.private.volume

Kirigami.Page {
    id: volumeControlsConfigPage
    title: i18nc("@title:window page title", "Volume Controls")

    property var config

    VolumeFeedback {
        id: feedback
    }

    Kirigami.Form {
        Kirigami.FormGroup {
            Kirigami.FormEntry {
                title: i18nc("@label:spinbox how much e.g. keyboard shortcuts change volume, in percent", "Volume change step size:")
                contentItem: QQC2.SpinBox {
                    id: volumeStep
                    // So it doesn't resize itself when showing a 2 or 3-digit number
                    Layout.minimumWidth: Kirigami.Units.gridUnit * 3
                    value: config.volumeStep
                    onValueModified: { config.volumeStep = value; config.save() }
                    from: 1
                    to: 100
                    stepSize: 1
                    editable: true
                    textFromValue: function(value) {
                        return value + "%";
                    }
                    valueFromText: function(text) {
                        return parseInt(text);
                    }

                    validator: PercentValidator {
                        bottom: Math.min(volumeStep.from, volumeStep.to)
                        top: Math.max(volumeStep.from, volumeStep.to)
                    }
                }
            }

            Kirigami.FormSeparator {}

            Kirigami.FormEntry {
                title: i18nc("@option:check prefix completes … Raise above 100%", "Maximum volume:")
                contentItem: QQC2.CheckBox {
                    checked: config.raiseMaximumVolume
                    text: i18nc("@option:check completes Maximum volume: …", "Raise above 100%")
                    onToggled: { config.raiseMaximumVolume = checked; config.save() }
                }
                subtitle: i18nc("@info:usagetip", "Prolonged use of this feature will damage the device's speakers.")
            }
        }
        Kirigami.FormGroup {
            Kirigami.FormEntry {
                title: i18nc("@option:check prefix, completes … Audio volume", "Play audio feedback for changes to:")
                contentItem: QQC2.CheckBox {
                    id: volumeFeedback
                    checked: config.audioFeedback
                    onToggled: { config.audioFeedback = checked; config.save() }
                    text: i18nc("@option:check, completes Play audio feedback for changes to:", "Audio volume")
                    enabled: feedback.valid
                }
            }

            Kirigami.FormSeparator {}

            Kirigami.FormEntry {
                title: i18nc("@title:group completes e.g. … Audio volume", "Show OSD popups for changes to:")
                contentItem: QQC2.CheckBox {
                    id: volumeOsd
                    checked: config.volumeOsd
                    onToggled: { config.volumeOsd = checked; config.save() }
                    text: i18nc("@option:check completes Show OSD popups for changes to:", "Audio volume")
                }
            }
            Kirigami.FormEntry {
                contentItem: QQC2.CheckBox {
                    id: micOsd
                    checked: config.microphoneSensitivityOsd
                    onToggled: { config.microphoneSensitivityOsd = checked; config.save() }
                    text: i18nc("@option:check completes Show OSD popups for changes to:", "Microphone sensitivity")
                }
            }
            Kirigami.FormEntry {
                contentItem: QQC2.CheckBox {
                    id: muteOsd
                    checked: config.muteOsd
                    onToggled: { config.muteOsd = checked; config.save() }
                    text: i18nc("@option:check completes Show OSD popups for changes to:", "Mute state")
                }
            }
            Kirigami.FormEntry {
                contentItem: QQC2.CheckBox {
                    id: pushToTalkOsd
                    checked: config.pushToTalkOsd
                    onToggled: { config.pushToTalkOsd = checked; config.save() }
                    text: i18nc("@option:check completes Show OSD popups for changes to:", "Push to talk")
                }
            }
            Kirigami.FormEntry {
                contentItem: QQC2.CheckBox {
                    checked: config.mutedMicrophoneReminderOsd
                    onToggled: { config.mutedMicrophoneReminderOsd = checked; config.save() }
                    text: i18nc("@option:check completes Show OSD popups for changes to:", "Application starts using muted microphone")
                }
            }
            Kirigami.FormEntry {
                contentItem: QQC2.CheckBox {
                    id: outputChangeOsd
                    checked: config.defaultOutputDeviceOsd
                    onToggled: { config.defaultOutputDeviceOsd = checked; config.save() }
                    text: i18nc("@option:check completes Show OSD popups for changes to:", "Default output device")
                }
            }
        }
    }
}
