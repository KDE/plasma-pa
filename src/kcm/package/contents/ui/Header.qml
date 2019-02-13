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
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.0

ColumnLayout {
    property alias text: heading.text
    property alias disabledText: disabledLabel.text

    Item {
        Layout.fillWidth: true
        Layout.preferredHeight: units.gridUnit * 1.5
        Layout.topMargin: units.smallSpacing

        Label {
            id: heading
            anchors.centerIn: parent
            font.weight: Font.DemiBold
        }
    }

    Label {
        id: disabledLabel
        Layout.alignment: Qt.AlignCenter
        visible: text && !parent.enabled
        font.italic: true
    }
}
