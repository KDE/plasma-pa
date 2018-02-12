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
        switch(FormFactor) {
            case "internal":
                return "audio-card";
            case "speaker":
                return "audio-speakers-symbolic";
            case "phone":
                return "phone";
            case "handset":
                return "phone";
            case "tv":
                return "video-television";
            case "webcam":
                return "camera-web";
            case "microphone":
                return "audio-input-microphone";
            case "headset":
                return "audio-headset";
            case "headphone":
                return "audio-headphones";
/*  There are some form factors which we don't have any icon for, so we ignore them for now */
//          case "hands-free": break;
//          case "car": break;
//          case "hifi": break;
            case "computer":
                return "computer"; break;
//          case "portable": break;
        }
        return IconName;
    }
}
