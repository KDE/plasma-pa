/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>
    SPDX-FileCopyrightText: 2016 David Rosca <nowrep@gmail.com>
    SPDX-FileCopyrightText: 2019 Sefa Eyeoglu <contact@scrumplex.net>
    SPDX-FileCopyrightText: 2020 Nicolas Fella <nicolas.fella@gmx.de>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick 2.7
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.0

import org.kde.kcm 1.3
import org.kde.kirigami 2.12 as Kirigami
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

    footer: ColumnLayout {
        // FIXME: Settings randomly do not appear
        CheckBox {
            Layout.fillWidth: true
            Layout.topMargin: Kirigami.Units.smallSpacing
            Layout.leftMargin: Kirigami.Units.gridUnit / 2
            Layout.rightMargin: Kirigami.Units.gridUnit / 2
            text: i18nd("kcm_pulseaudio", "Add virtual output device for simultaneous output on all local sound cards")
            checked: moduleManager.combineSinks
            onToggled: moduleManager.combineSinks = checked;
            enabled: moduleManager.configModuleLoaded
            visible: moduleManager.settingsSupported
        }

        CheckBox {
            Layout.fillWidth: true
            Layout.leftMargin: Kirigami.Units.gridUnit / 2
            Layout.rightMargin: Kirigami.Units.gridUnit / 2
            text: i18nd("kcm_pulseaudio", "Automatically switch all running streams when a new output becomes available")
            checked: moduleManager.switchOnConnect
            onToggled: moduleManager.switchOnConnect = checked;
            enabled: moduleManager.configModuleLoaded
            visible: moduleManager.settingsSupported
        }

        Label {
            Layout.alignment: Qt.AlignHCenter
            enabled: false
            font.italic: true
            text: i18nd("kcm_pulseaudio", "Requires %1 PulseAudio module", moduleManager.configModuleName)
            visible: moduleManager.settingsSupported && !moduleManager.configModuleLoaded
        }

        Button {
            id: inactiveDevicesButton
            Layout.alignment: Qt.AlignRight
            checkable: true
            text: i18nd("kcm_pulseaudio", "Show Inactive Devices")
            icon.name: "view-visible"

            // Only show if there actually are any inactive devices
            visible: (paSourceModel.count != paSourceFilterModel.count) || (paSinkModel.count != paSinkFilterModel.count)
        }
    }


    Kirigami.OverlaySheet {
        id: testOverlay

        property string description: ""

        function testSink(index) {
            let modelIndex = sinks.model.index(Math.max(index, 0), 0);
            testItem.sinkObject = sinks.model.data(modelIndex, sinks.model.role("PulseObject"));
            testOverlay.description = sinks.model.data(modelIndex, sinks.model.role("Description"));
            testOverlay.open();
        }

        header: Label {
            text: i18nd("kcm_pulseaudio", "Testing %1", testOverlay.description)
        }

        SinkTest {
            id: testItem
            Layout.fillWidth: true
        }
    }
}
