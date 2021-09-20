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

    property var pulseObject: model.PulseObject

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

        Button {
            id: testButton
            text: i18ndc("kcm_pulseaudio", "Perform an audio test of the device", "Test")
            icon.name: "audio-speakers-symbolic"
            visible: isPlayback
            onClicked: testOverlay.testSink(index);
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
                delegate.pulseObject.channelVolumes = volumes;
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
        Item {
            Layout.leftMargin: Kirigami.Units.largeSpacing * 3
            Layout.fillWidth: true
        }

        Label {
            id: portLabel
            visible: portBox.visible
            text: i18nd("kcm_pulseaudio", "Port:")
            Layout.leftMargin: Kirigami.Units.largeSpacing * 2
        }

        ComboBox {
            id: portBox
            readonly property var ports: Ports
            visible: portBox.count > 1
            onModelChanged: currentIndex = ActivePortIndex
            currentIndex: ActivePortIndex
            onActivated: ActivePortIndex = index

            onPortsChanged: {
                var items = [];
                for (var i = 0; i < ports.length; ++i) {
                    var port = ports[i];
                    var text;
                    if (port.availability == Port.Unavailable) {
                        if (port.name == "analog-output-speaker" || port.name == "analog-input-microphone-internal") {
                            text = i18ndc("kcm_pulseaudio", "Port is unavailable", "%1 (unavailable)", port.description);
                        } else {
                            text = i18ndc("kcm_pulseaudio", "Port is unplugged", "%1 (unplugged)", port.description);
                        }
                    } else {
                        text = port.description;
                    }
                    items.push(text);
                }
                model = items;
            }
        }

        Label {
            id: profileLabel
            visible: profileBox.visible
            text: i18ndc("kcm_pulseaudio", "@label", "Profile:")
            Layout.leftMargin: Kirigami.Units.largeSpacing * 2
        }

        ComboBox {
            id: profileBox

            readonly property var card: paCardModel.data(paCardModel.indexOfCardNumber(CardIndex), paCardModel.role("PulseObject"))

            visible: profileBox.count > 1
            textRole: "description"

            model: card ? card.profiles.filter(profile => profile.availability === Profile.Available) : []
            currentIndex: card ? model.indexOf(card.profiles[card.activeProfileIndex]) : -1

            onActivated: card.activeProfileIndex = card.profiles.indexOf(model[index])
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
                        delegate.pulseObject.setChannelVolume(index, value);

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

    Kirigami.Separator {
        visible: (delegate.ListView.view.count != 0) && (delegate.ListView.view.count != (index + 1))
        Layout.topMargin: delegate.ListView.view.spacing - delegate.spacing
        Layout.fillWidth: true
    }
}
