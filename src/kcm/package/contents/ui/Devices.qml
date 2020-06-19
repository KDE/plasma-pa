/*
    Copyright 2014-2015 Harald Sitter <sitter@kde.org>
    Copyright 2019 Sefa Eyeoglu <contact@scrumplex.net>

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

import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.3

import org.kde.kirigami 2.5 as Kirigami

import org.kde.plasma.private.volume 0.1

ScrollView {
    id: scrollView

    contentHeight: contentItem.height
    clip: true

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

    Item {
    id: contentItem
    width: scrollView.availableWidth
    height: contentLayout.implicitHeight

        ColumnLayout {
            id: contentLayout
            width: scrollView.availableWidth

            Header {
                Layout.fillWidth: true
                enabled: sinks.count > 0
                text: i18nd("kcm_pulseaudio", "Playback Devices")
                disabledText: i18ndc("kcm_pulseaudio", "@label", "No Playback Devices Available")
            }

            ListView {
                id: sinks
                Layout.fillWidth: true
                Layout.preferredHeight: contentHeight
                Layout.margins: Kirigami.Units.gridUnit / 2
                interactive: false
                spacing: Kirigami.Units.gridUnit
                model: inactiveDevicesButton.checked ? paSinkModel : paSinkFilterModel
                delegate: DeviceListItem {
                    isPlayback: true
                }
            }

            Header {
                Layout.fillWidth: true
                enabled: sources.count > 0
                text: i18nd("kcm_pulseaudio", "Recording Devices")
                disabledText: i18ndc("kcm_pulseaudio", "@label", "No Recording Devices Available")
            }

            ListView {
                id: sources
                Layout.fillWidth: true
                Layout.preferredHeight: contentHeight
                Layout.margins: Kirigami.Units.gridUnit / 2
                interactive: false
                spacing: Kirigami.Units.gridUnit
                model: inactiveDevicesButton.checked ? paSourceModel : paSourceFilterModel
                delegate: DeviceListItem {
                    isPlayback: false
                }
            }

            Button {
                id: inactiveDevicesButton

                Layout.alignment: Qt.AlignHCenter
                Layout.margins: Kirigami.Units.gridUnit

                checkable: true

                text: i18nd("kcm_pulseaudio", "Show Inactive Devices")
                icon.name: "view-visible"

                // Only show if there actually are any inactive devices
                visible: (paSourceModel.rowCount != paSourceFilterModel.rowCount) || (paSinkModel.rowCount != paSinkFilterModel.rowCount)
            }
        }
    }
}
