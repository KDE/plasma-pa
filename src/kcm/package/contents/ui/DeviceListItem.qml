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

ColumnLayout {
    id: delegate
    width: parent.width

    RowLayout {
        QIconItem {
            Layout.alignment: Qt.AlignBottom
            width: height
            height: inputText.height
            icon: 'audio-card'
        }

        Label {
            id: inputText
            Layout.alignment: Qt.AlignBottom
            Layout.fillWidth: true
            elide: Text.ElideRight
            text: PulseObject.description
        }

        MuteButton {
            muted: PulseObject.muted
            onCheckedChanged: PulseObject.muted = checked
        }
    }

    ColumnLayout {
        width: parent.width
        enabled: !PulseObject.muted

        RowLayout {
            visible: portbox.count > 1

            Label {
                text: 'Port'
            }

            ComboBox {
                id: portbox
                Layout.fillWidth: true
                model: PulseObject.ports
                onModelChanged: currentIndex = PulseObject.activePortIndex
                textRole: "description"
                currentIndex: PulseObject.activePortIndex
                onActivated: PulseObject.activePortIndex = index
            }
        }

        VolumeSlider {}
    }

    ListItemSeperator { view: delegate.ListView.view }
}
