/*
    Copyright 2014-2015 Harald Sitter <sitter@kde.org>

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
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

import org.kde.kquickcontrolsaddons 2.0
import org.kde.plasma.core 2.0 as PlasmaCore /* for units.gridUnit */

ScrollView {
    property alias model: view.model
    property alias delegate: view.delegate
    property alias emptyText: emptyLabel.text

    frameVisible: false
    highlightOnFocus: true
    anchors.fill: parent

    ListView {
        id: view
        visible: count > 0
        anchors.fill: parent
        anchors.margins: units.gridUnit / 2
        spacing: units.largeSpacing
        interactive: false // Not flickable
    }


    Item {
        visible: !view.visible
        width: viewport.width
        height: viewport.height

        ColumnLayout {
            anchors.centerIn: parent

            QIconItem {
                Layout.preferredWidth: units.iconSizes.medium
                Layout.preferredHeight: units.iconSizes.medium
                Layout.alignment: Qt.AlignHCenter
                icon: 'dialog-information'
            }

            Label {
                id: emptyLabel
                Layout.alignment: Qt.AlignHCenter
            }
        }
    }
}
