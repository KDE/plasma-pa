/*
    Copyright 2014-2015 Harald Sitter <sitter@kde.org>
    Copyright 2016 David Rosca <nowrep@gmail.com>
    Copyright 2019 Sefa Eyeoglu <contact@scrumplex.net>
    Copyright 2020 Nicolas Fella <nicolas.fella@gmx.de>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of
    the License or (at your option) version 3 or any later version
    accepted by the membership of KDE e.V. (or its successor approved
    by the membership of KDE e.V.), which shall act as a proxy
    defined in Section 14 of version 3 of the license.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

import QtQuick 2.7
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.0

import org.kde.kcm 1.3
import org.kde.kirigami 2.12 as Kirigami
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
                spacing: Kirigami.Units.gridUnit
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
                spacing: Kirigami.Units.gridUnit
                model: inactiveDevicesButton.checked || !inactiveDevicesButton.visible ? paSourceModel : paSourceFilterModel
                delegate: DeviceListItem {
                    isPlayback: false
                }
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

        Button {
            id: inactiveDevicesButton
            checkable: true
            text: i18nd("kcm_pulseaudio", "Show Inactive Devices")
            icon.name: "view-visible"

            // Only show if there actually are any inactive devices
            visible: (paSourceModel.count != paSourceFilterModel.count) || (paSinkModel.count != paSinkFilterModel.count)
        }

        Button {
            text: i18n("Configure...")
            icon.name: "configure"
            Layout.alignment: Qt.AlignRight
            onClicked: kcm.push("Advanced.qml")
        }
    }
}
