/*
    SPDX-FileCopyrightText: 2026 Ramil Nurmanov <ramil2004nur@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.Layouts
import org.kde.plasma.components as PlasmaComponents3
import org.kde.kirigami as Kirigami
import org.kde.plasma.private.volume

Item {
    id: root

    property var model: null
    property var sourceObject: null
    property bool showInternalBackButton: true
    property string description: ""
    property string iconName: "audio-input-microphone-symbolic"
    property string port: ""
    property string profile: ""

    signal backRequested()

    onModelChanged: {
        if (model) {
            sourceObject = model.PulseObject;
            micTester.source = model.PulseObject;
            description = model.Description || "";
            iconName = model.IconName || "audio-input-microphone-symbolic";

            const ports = model.Ports || [];
            if (ports.length > 1) {
                const activeIndex = model.ActivePortIndex;
                port = ports[activeIndex] ? ports[activeIndex].description : "";
            } else {
                port = "";
            }
        } else {
            sourceObject = null;
        }
    }

    function stopTest() {
        if (micTester.recording) {
            micTester.stopRecording();
        }
        if (micTester.playing) {
            micTester.stopPlaying();
        }
        micTester.clearRecording();
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Kirigami.Units.largeSpacing
        spacing: Kirigami.Units.largeSpacing * 3

        Kirigami.InlineMessage {
            id: micTestError
            type: Kirigami.MessageType.Error
            showCloseButton: true
            Layout.fillWidth: true
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: Kirigami.Units.smallSpacing

            Kirigami.Icon {
                source: root.iconName
                implicitWidth: Kirigami.Units.iconSizes.medium
                implicitHeight: Kirigami.Units.iconSizes.medium
                Layout.alignment: Qt.AlignCenter
            }

            Kirigami.Heading {
                text: root.description
                level: 2
                Layout.fillWidth: true
                wrapMode: Text.Wrap
            }

            PlasmaComponents3.Label {
                text: {
                    if (root.port.length === 0)
                        return root.profile;
                    if (root.profile.length === 0)
                        return root.port;
                    return root.profile + " / " + root.port;
                }
                visible: text.length > 0
                Layout.fillWidth: true
                elide: Text.ElideRight
            }
        }

        RowLayout {
            spacing: Kirigami.Units.largeSpacing
            Layout.alignment: Qt.AlignHCenter

            PlasmaComponents3.Button {
                text: micTester.recording ? i18nc("Button to stop microphone recording in test mode", "Stop") : i18nc("Button to start microphone recording in test mode", "Record")
                icon.name: micTester.recording ? "media-playback-stop-symbolic" : "media-record-symbolic"
                enabled: !micTester.playing

                onClicked: {
                    micTestError.visible = false;
                    if (micTester.recording)
                        micTester.stopRecording();
                    else
                        micTester.startRecording();
                }
            }

            PlasmaComponents3.Button {
                text: micTester.playing ? i18nc("Button to stop microphone playing in test mode", "Stop") : i18nc("Button to start microphone playing in test mode", "Play")
                icon.name: micTester.playing ? "media-playback-stop-symbolic" : "media-playback-start-symbolic"
                enabled: micTester.hasRecording && !micTester.recording

                onClicked: {
                    micTestError.visible = false;
                    if (micTester.playing)
                        micTester.stopPlaying()
                    else
                        micTester.playRecording();
                }
            }
        }

        ColumnLayout {
            spacing: 0
            Layout.fillWidth: true

            PlasmaComponents3.Label {
                text: i18nc("Label above the microphone volume meter in test overlay", "Volume level")
                font: Kirigami.Theme.smallFont
                opacity: 0.75
                Layout.fillWidth: true
                elide: Text.ElideRight
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: Kirigami.Units.smallSpacing

                PlasmaComponents3.ProgressBar {
                    id: volumeProgressBar
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignVCenter

                    from: 0
                    to: 1
                    value: micTester.recording ? micTester.volumeLevel : 0

                    Behavior on value {
                        NumberAnimation {
                            duration: Kirigami.Units.longDuration
                            easing.type: Easing.OutQuad
                        }
                    }
                }

                PlasmaComponents3.Label {
                    text: i18nc("Label showing current microphone input level percentage in test mode", "%1 %", Math.round(volumeProgressBar.value * 100))
                    font: Kirigami.Theme.smallFont
                    Layout.alignment: Qt.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    Layout.minimumWidth: valueTextMetrics.advanceWidth
                    Layout.maximumWidth: valueTextMetrics.advanceWidth
                }
            }
        }

        ColumnLayout {
            spacing: 0
            Layout.fillWidth: true

            PlasmaComponents3.Label {
                text: i18nc("Label above the microphone sensitivity slider in test overlay", "Sensitivity")
                font: Kirigami.Theme.smallFont
                opacity: 0.75
                Layout.fillWidth: true
                elide: Text.ElideRight
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: Kirigami.Units.smallSpacing

                VolumeSlider {
                    id: inputLevelSlider
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignVCenter

                    volumeObject: root.sourceObject
                    showVolumeIndicator: false
                    muted: root.sourceObject ? root.sourceObject.muted : false

                    from: PulseAudio.MinimalVolume
                    to: config.raiseMaximumVolume ? PulseAudio.MaximalVolume : PulseAudio.NormalVolume
                    value: root.sourceObject ? root.sourceObject.volume : PulseAudio.NormalVolume
                    enabled: root.sourceObject && root.sourceObject.volumeWritable && !micTester.playing && !micTester.recording

                    onMoved: {
                        if (!root.sourceObject) return;
                        const volume = Math.round(value * 100 / PulseAudio.NormalVolume) * PulseAudio.NormalVolume / 100;
                        root.sourceObject.volume = volume;
                        root.sourceObject.muted = volume === 0;
                    }
                }

                PlasmaComponents3.Label {
                    text: i18ndc("kcm_pulseaudio", "volume percentage", "%1%", Math.round(inputLevelSlider.value / PulseAudio.NormalVolume * 100))
                    Layout.alignment: Qt.AlignVCenter
                    font: Kirigami.Theme.smallFont
                    horizontalAlignment: Text.AlignHCenter
                    Layout.minimumWidth: valueTextMetrics.advanceWidth
                    Layout.maximumWidth: valueTextMetrics.advanceWidth
                }
            }
        }

        TextMetrics {
            id: valueTextMetrics
            font: Kirigami.Theme.smallFont
            text: i18ndc("kcm_pulseaudio", "only used for sizing, should be widest possible string", "100%")
        }

        Item {
            Layout.fillHeight: true
        }

        PlasmaComponents3.Button {
            visible: root.showInternalBackButton
            text: i18nc("Button in microphone test view to return to the device list", "Back")
            icon.name: "go-previous-symbolic"
            Layout.alignment: Qt.AlignLeft | Qt.AlignBottom
            Layout.bottomMargin: Kirigami.Units.largeSpacing
            Layout.topMargin: Kirigami.Units.mediumSpacing
            onClicked: root.backRequested()
        }
    }

    MicrophoneTest {
        id: micTester

        onShowErrorMessage: (message) => {
            micTestError.text = xi18nc("@info", "Microphone test error:<nl/><nl/>%1", message);
            micTestError.visible = true;
        }
    }
}