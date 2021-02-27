/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>
    SPDX-FileCopyrightText: 2019 Sefa Eyeoglu <contact@scrumplex.net>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

import org.kde.kirigami 2.5 as Kirigami
import org.kde.plasma.private.volume 0.1

ColumnLayout {
    id: delegate
    spacing: Kirigami.Units.smallSpacing * 2
    width: parent.width

    property bool isPlayback: type.substring(0, 4) == "sink"

    readonly property var currentPort: Ports[ActivePortIndex]

    opacity: (Ports.length === 1 && Ports[0].availability == Port.Unavailable) ? 0.5 : 1

    RowLayout {
        spacing: Kirigami.Units.smallSpacing
        Layout.fillWidth: true

        RadioButton {
            id: defaultButton
            Layout.fillWidth: true
            Layout.leftMargin: LayoutMirroring.enabled ? 0 : Math.round((muteButton.width - defaultButton.indicator.width) / 2)
            Layout.rightMargin: LayoutMirroring.enabled ? Math.round((muteButton.width - defaultButton.indicator.width) / 2) : 0
            spacing: Kirigami.Units.smallSpacing + Math.round((muteButton.width - defaultButton.indicator.width) / 2)
            checked: Default
            visible: delegate.ListView.view.count > 1
            onClicked: Default = true
            text: !currentPort ? Description : i18ndc("kcm_pulseaudio", "label of device items", "%1 (%2)", currentPort.description, Description)
        }

        Label {
            id: soloLabel
            Layout.fillWidth: true
            text: defaultButton.text
            visible: delegate.ListView.view.count <= 1
            elide: Text.ElideRight
        }

        Label {
            id: portLabel
            visible: portbox.visible
            text: i18nd("kcm_pulseaudio", "Port:")
        }

        ComboBox {
            id: portbox
            readonly property var ports: Ports
            visible: portbox.count > 1 && delegate.width - Kirigami.Units.gridUnit * 8 > implicitWidth
            onModelChanged: currentIndex = ActivePortIndex
            currentIndex: ActivePortIndex
            onActivated: ActivePortIndex = index

            onPortsChanged: {
                var items = [];
                for (var i = 0; i < ports.length; ++i) {
                    var port = ports[i];
                    var text = port.description;
                    if (port.availability == Port.Unavailable) {
                        if (port.name == "analog-output-speaker" || port.name == "analog-input-microphone-internal") {
                            text += i18ndc("kcm_pulseaudio", "Port is unavailable", " (unavailable)");
                        } else {
                            text += i18ndc("kcm_pulseaudio", "Port is unplugged", " (unplugged)");
                        }
                    }
                    items.push(text);
                }
                model = items;
            }
        }

        Button {
            id: balanceButton
            text: i18ndc("kcm_pulseaudio", "Audio balance (e.g. control left/right volume individually", "Balance")
            icon.name: "view-media-equalizer"
            checkable: true

            onClicked: {
                if (checked) {
                    return;
                }

                // When unifying channels again, set all of them to the same value
                // as to not keep an odd difference when adjusting them in unison
                let volumes = ChannelVolumes;
                for (let i = 0, count = RawChannels.length; i < count; ++i) {
                    volumes[i] = Volume;
                }
                // NOTE "ChannelVolumes = volumes" does not work as the
                // AbstractModel does not have the magic JS Array to Qt List
                // conversion stuff and only sees our Array as a QJSValue
                PulseObject.channelVolumes = volumes;
            }

            // Default to individual mode when a channel has a different volume
            Component.onCompleted: {
                checked = ChannelVolumes.some((volume) => {
                    return volume !== ChannelVolumes[0];
                });
            }
        }
    }

    RowLayout {
        spacing: Kirigami.Units.smallSpacing

        MuteButton {
            id: muteButton
            Layout.alignment: Qt.AlignTop
            Layout.topMargin: {
                // Center the mute button around the main slider, or the first channel slider
                const sliderHeight = channelSliderRepeater.count > 0 ? channelSliderRepeater.itemAt(0).height : volumeSlider.height;
                return -Math.round((height - sliderHeight) / 2);
            }
            muted: Muted
            onCheckedChanged: Muted = checked
            toolTipText: !currentPort ? Description : currentPort.description
        }

        GridLayout {
            rows: Math.max(1, channelSliderRepeater.count)
            flow: GridLayout.TopToBottom

            VolumeSlider {
                id: volumeSlider
                Layout.columnSpan: 2
                Layout.alignment: Qt.AlignTop
                visible: !balanceButton.checked

                value: Volume
                onMoved: {
                    Volume = value;
                    Muted = (value === 0);
                }
            }

            Repeater {
                model: balanceButton.checked ? Channels : null

                Label {
                    Layout.alignment: Qt.AlignTop | Qt.AlignRight
                    text: i18ndc("kcm_pulseaudio", "Placeholder is channel name", "%1:", modelData)
                }
            }

            Repeater {
                id: channelSliderRepeater
                model: balanceButton.checked ? RawChannels : null

                VolumeSlider {
                    Layout.fillWidth: true
                    hundredPercentLabelVisible: index === channelSliderRepeater.count -1

                    value: ChannelVolumes[index]
                    onMoved: {
                        PulseObject.setChannelVolume(index, value);

                        // volumes are updated async, so we'll just assume it worked here
                        let newChannelVolumes = ChannelVolumes;
                        newChannelVolumes[index] = value;
                        Muted = newChannelVolumes.every((volume) => {
                            return volume === 0;
                        });
                    }
                }
            }
        }
    }
}
