/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>
    SPDX-FileCopyrightText: 2016 David Rosca <nowrep@gmail.com>
    SPDX-FileCopyrightText: 2019 Sefa Eyeoglu <contact@scrumplex.net>
    SPDX-FileCopyrightText: 2020 Nicolas Fella <nicolas.fella@gmx.de>
    SPDX-FileCopyrightText: 2021 Ismael Asensio <isma.af@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick 2.7
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.0

import org.kde.kcoreaddons 1.0 as KCoreAddons
import org.kde.kcm 1.3
import org.kde.kirigami 2.13 as Kirigami
import org.kde.plasma.core 2.1 as PlasmaCore
import org.kde.plasma.private.volume 0.1

ScrollViewKCM {

    property int maxVolumeValue: PulseAudio.NormalVolume // the applet supports changing this value. We will just assume 65536 (100%)
    ConfigModule.quickHelp: i18nd("kcm_pulseaudio", "This module allows configuring the Pulseaudio sound subsystem.")

    implicitHeight: Kirigami.Units.gridUnit * 28
    implicitWidth: Kirigami.Units.gridUnit * 28

    SinkModel {
        id: paSinkModel
    }

    SourceModel {
        id: paSourceModel
    }

    CardModel {
        id: paCardModel

        function indexOfCardNumber(cardNumber) {
            for (var idx = 0; idx < count; idx++) {
                if (data(index(idx, 0), role("Index")) == cardNumber) {
                    return index(idx, 0);
                }
            }
            return index(-1, 0);
        }
    }

    PulseObjectFilterModel {
        id: paSinkFilterModel

        sortRole: "SortByDefault"
        sortOrder: Qt.DescendingOrder
        filterOutInactiveDevices: true
        sourceModel: paSinkModel
    }

    PulseObjectFilterModel {
        id: paSourceFilterModel

        sortRole: "SortByDefault"
        sortOrder: Qt.DescendingOrder
        filterOutInactiveDevices: true
        sourceModel: paSourceModel
    }

    ModuleManager {
        id: moduleManager
    }

    view: Flickable {
        id: flickable

        contentWidth: column.width
        contentHeight: column.height
        clip: true

        ColumnLayout {
            id: column
            width: flickable.width

            Kirigami.ListSectionHeader {
                Layout.fillWidth: true
                visible: sinks.count > 0
                text: i18nd("kcm_pulseaudio", "Playback Devices")
            }

            ListView {
                id: sinks
                Layout.fillWidth: true
                Layout.leftMargin: Kirigami.Units.largeSpacing
                Layout.rightMargin: Kirigami.Units.largeSpacing
                Layout.preferredHeight: contentHeight
                interactive: false
                spacing: Kirigami.Units.largeSpacing
                model: inactiveDevicesButton.checked || !inactiveDevicesButton.visible ? paSinkModel : paSinkFilterModel
                delegate: DeviceListItem {
                    isPlayback: true
                }
            }

            Kirigami.ListSectionHeader {
                Layout.fillWidth: true
                visible: sources.count > 0
                text: i18nd("kcm_pulseaudio", "Recording Devices")
            }

            ListView {
                id: sources
                Layout.fillWidth: true
                Layout.leftMargin: Kirigami.Units.largeSpacing
                Layout.rightMargin: Kirigami.Units.largeSpacing
                Layout.preferredHeight: contentHeight
                interactive: false
                spacing: Kirigami.Units.largeSpacing
                model: inactiveDevicesButton.checked || !inactiveDevicesButton.visible ? paSourceModel : paSourceFilterModel
                delegate: DeviceListItem {
                    isPlayback: false
                }
            }

            Kirigami.ListSectionHeader {
                Layout.fillWidth: true
                visible: inactiveCards.count > 0
                text: i18nd("kcm_pulseaudio", "Inactive Cards")
            }

            ListView {
                id: inactiveCards
                Layout.fillWidth: true
                Layout.preferredHeight: contentHeight
                Layout.margins: Kirigami.Units.gridUnit / 2
                interactive: false
                spacing: Kirigami.Units.smallSpacing * 2

                model: PlasmaCore.SortFilterModel {
                    sourceModel: paCardModel
                    function role(name) {
                        return sourceModel.role(name);
                    }
                    filterCallback: function(source_row, value) {
                        let idx = sourceModel.index(source_row, 0);
                        let profiles = sourceModel.data(idx, sourceModel.role("Profiles"))
                        let activeProfileIndex = sourceModel.data(idx, sourceModel.role("ActiveProfileIndex"))
                        return profiles[activeProfileIndex].name == "off";
                    }
                }
                delegate: CardListItem {}
            }

            Kirigami.ListSectionHeader {
                Layout.fillWidth: true
                visible: eventStreamView.count || sinkInputView.count
                text: i18nd("kcm_pulseaudio", "Playback Streams")
            }

            ListView {
                id: eventStreamView
                Layout.fillWidth: true
                Layout.leftMargin: Kirigami.Units.largeSpacing
                Layout.rightMargin: Kirigami.Units.largeSpacing
                Layout.preferredHeight: contentHeight
                interactive: false
                spacing: Kirigami.Units.largeSpacing
                model: PulseObjectFilterModel {
                    filters: [ { role: "Name", value: "sink-input-by-media-role:event" } ]
                    sourceModel: StreamRestoreModel {}
                }
                delegate: StreamListItem {
                    deviceModel: paSinkModel
                    isPlayback: true
                }
            }

            ListView {
                id: sinkInputView
                Layout.fillWidth: true
                Layout.leftMargin: Kirigami.Units.largeSpacing
                Layout.rightMargin: Kirigami.Units.largeSpacing
                Layout.preferredHeight: contentHeight
                interactive: false
                spacing: Kirigami.Units.largeSpacing
                model: PulseObjectFilterModel {
                    filters: [ { role: "VirtualStream", value: false } ]
                    sourceModel: SinkInputModel {}
                }
                delegate: StreamListItem {
                    deviceModel: paSinkModel
                    isPlayback: true
                }
            }

            Kirigami.ListSectionHeader {
                Layout.fillWidth: true
                visible: sourceOutputView.count > 0
                text: i18nd("kcm_pulseaudio", "Recording Streams")
            }

            ListView {
                id: sourceOutputView
                Layout.fillWidth: true
                Layout.leftMargin: Kirigami.Units.largeSpacing
                Layout.rightMargin: Kirigami.Units.largeSpacing
                Layout.preferredHeight: contentHeight
                interactive: false
                spacing: Kirigami.Units.largeSpacing
                model: PulseObjectFilterModel {
                    filters: [ { role: "VirtualStream", value: false } ]
                    sourceModel: SourceOutputModel {}
                }

                delegate: StreamListItem {
                    deviceModel: sourceModel
                    isPlayback: false
                }
            }
        }
    }

    footer: RowLayout {
        Item {
            Layout.fillWidth: true
        }

        Button {
            id: inactiveDevicesButton
            checkable: true
            text: i18nd("kcm_pulseaudio", "Show Inactive Devices")
            icon.name: "view-visible"

            // Only show if there actually are any inactive devices
            visible: (paSourceModel.count != paSourceFilterModel.count) || (paSinkModel.count != paSinkFilterModel.count)
        }

        Button {
            id: configureButton
            visible: moduleManager.settingsSupported
            enabled: moduleManager.configModuleLoaded
            text: i18nd("kcm_pulseaudio", "Configure…")
            icon.name: "configure"

            ToolTip.visible: !enabled && hovered
            ToolTip.text: i18nd("kcm_pulseaudio", "Requires %1 PulseAudio module", moduleManager.configModuleName)

            onClicked: configureMenu.open()

            Menu {
                id: configureMenu
                y: -height - Kirigami.Units.smallSpacing

                MenuItem {
                    text: i18nd("kcm_pulseaudio", "Add virtual output device for simultaneous output on all local sound cards")
                    checkable: true
                    checked: moduleManager.combineSinks
                    onToggled: moduleManager.combineSinks = checked;
                }
                MenuItem {
                    text: i18nd("kcm_pulseaudio", "Automatically switch all running streams when a new output becomes available")
                    checkable: true
                    checked: moduleManager.switchOnConnect
                    onToggled: moduleManager.switchOnConnect = checked;
                }
            }
        }
    }


    Kirigami.OverlaySheet {
        id: testOverlay

        property var sinkObject: null
        property string description: ""
        property string iconName: "audio-card"
        property string profile: ""
        property string port: ""

        function testSink(index) {
            let modelIndex = sinks.model.index(Math.max(index, 0), 0);
            sinkObject = sinks.model.data(modelIndex, sinks.model.role("PulseObject"));
            description = sinks.model.data(modelIndex, sinks.model.role("Description"));
            iconName = sinks.model.data(modelIndex, sinks.model.role("IconName")) || "audio-card";

            let ports = sinks.model.data(modelIndex, sinks.model.role("Ports"));
            port = ports.length > 1 ? ports[sinks.model.data(modelIndex, sinks.model.role("ActivePortIndex"))].description : "";

            profile = "";
            let cardIndex = paCardModel.indexOfCardNumber(sinks.model.data(modelIndex, sinks.model.role("CardIndex")));
            if (cardIndex.valid) {
                let profiles = paCardModel.data(cardIndex, paCardModel.role("Profiles")) || [];
                profile = profiles.length > 1 ? profiles[paCardModel.data(cardIndex, paCardModel.role("ActiveProfileIndex"))].description : "";
            }

            testOverlay.open();
        }

        function channelData(channel) {
            switch (channel) {
                case "front-left": return {text: i18nd("kcm_pulseaudio", "Front Left"), row: 0, column: 0};
                case "front-center": return {text: i18nd("kcm_pulseaudio", "Front Center"), row: 0, column: 1};
                case "front-right": return {text: i18nd("kcm_pulseaudio", "Front Right"), row: 0, column: 2};
                case "side-left": return {text: i18nd("kcm_pulseaudio", "Side Left"), row: 1, column: 0};
                case "side-right": return {text: i18nd("kcm_pulseaudio", "Side Right"), row: 1, column: 2};
                case "rear-left": return {text: i18nd("kcm_pulseaudio", "Rear Left"), row: 2, column: 0};
                case "lfe": return {text: i18nd("kcm_pulseaudio", "Subwoofer"), row: 2, column: 1};
                case "rear-right": return {text: i18nd("kcm_pulseaudio", "Rear Right"), row: 2, column: 2};
                case "mono" : return {text: i18nd("kcm_pulseaudio", "Mono"), row: 0, column: 1};
            }
        }

        header: GridLayout {
            columns: 2
            rowSpacing: Kirigami.Units.smallSpacing

            Kirigami.Icon {
                source: testOverlay.iconName || "audio-card"
                Layout.rowSpan: 3
                Layout.alignment: Qt.AlignCenter
            }
            Label {
                text: testOverlay.description
                font.bold: true
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
            }
            Label {
                text: {
                    if (testOverlay.port.length === 0) { return testOverlay.profile }
                    if (testOverlay.profile.length === 0) { return testOverlay.port }
                    return testOverlay.profile + " / " + testOverlay.port
                }
                visible: text.length > 0
                Layout.fillWidth: true
                elide: Text.ElideRight
            }
        }

        GridLayout {
            id: layoutTest
            columns: 3
            LayoutMirroring.enabled: false  // To preserve spacial layout on RTL

            Kirigami.Avatar {
                KCoreAddons.KUser {
                    id: kuser
                }
                source: kuser.faceIconUrl
                sourceSize.width: Kirigami.Units.gridUnit * 3
                sourceSize.height: Kirigami.Units.gridUnit * 3
                Layout.row: 1
                Layout.column: 1
                Layout.alignment: Qt.AlignCenter

            }

            Repeater {
                model: testOverlay.sinkObject && testOverlay.sinkObject.rawChannels

                delegate: Button {
                    text: testOverlay.channelData(modelData).text
                    Layout.row: testOverlay.channelData(modelData).row
                    Layout.column: testOverlay.channelData(modelData).column
                    Layout.alignment: Qt.AlignCenter
                    Layout.fillWidth: true
                    Layout.preferredWidth: Kirigami.Units.gridUnit * 10
                    Layout.preferredHeight: Kirigami.Units.gridUnit * 2
                    // there is no subwoofer sound in the freedesktop theme https://gitlab.freedesktop.org/xdg/xdg-sound-theme/-/issues/7
                    onClicked: testOverlay.sinkObject.testChannel(modelData === "lfe" ? "rear-center" : modelData)
                }
            }
        }
    }
}
