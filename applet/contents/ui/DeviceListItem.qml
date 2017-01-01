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

ListItemBase {
    readonly property var currentPort: Ports[ActivePortIndex]

    draggable: false
    label: currentPort ? i18nc("label of device items", "%1 (%2)", currentPort.description, Description) : Description
    icon: {
        if (currentPort) {
            if (currentPort.name.indexOf("speaker") != -1) {
                return "audio-speakers-symbolic";
            } else if (currentPort.name.indexOf("headphones") != -1) {
                return "audio-headphones";
            } else if (currentPort.name.indexOf("hdmi") != -1) {
                return "video-television";
            } else if (currentPort.name.indexOf("mic") != -1) {
                return "audio-input-microphone";
            } else if (currentPort.name.indexOf("phone") != -1) {
                return "phone";
            }
        }
        return IconName;
    }
}
