/*
 *    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>
 *
 *    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

import QtQuick 2.0
import QtQuick.Controls 2.2

import org.kde.kcoreaddons 1.0 as KCoreAddons
import org.kde.kirigami 2.14 as Kirigami
import org.kde.plasma.private.volume 0.1

Grid {
    id: grid
    columns: 3
    spacing: 0

    property var sinkObject: null


    Item {
        width: grid.width/3
        height: 50

        Button{
            text: i18nd("kcm_pulseaudio", "Front Left")
            anchors.centerIn: parent
            visible: sinkObject ? sinkObject.rawChannels.indexOf("front-left") > -1 : false
            onClicked: sinkObject.testChannel("front-left")
        }
    }
    Item {
        width: grid.width/3
        height: 50

        Button{
            text: i18nd("kcm_pulseaudio", "Front Center")
            anchors.centerIn: parent
            visible: sinkObject ? sinkObject.rawChannels.indexOf("front-center") > -1 : false
            onClicked: sinkObject.testChannel("front-center")
        }
    }
    Item {
        width: grid.width/3
        height: 50

        Button{
            text: i18nd("kcm_pulseaudio", "Front Right")
            anchors.centerIn: parent
            visible: sinkObject ? sinkObject.rawChannels.indexOf("front-right") > -1 : false
            onClicked: sinkObject.testChannel("front-right")
        }
    }
    Item {
        width: grid.width/3
        height: 50

        Button{
            text: i18nd("kcm_pulseaudio", "Side Left")
            anchors.centerIn: parent
            visible: sinkObject ? sinkObject.rawChannels.indexOf("side-left") > -1 : false
            onClicked: sinkObject.testChannel("side-left")

        }
    }
    Item {
        width: grid.width/3
        height: 50

        KCoreAddons.KUser {
            id: kuser
        }

        Image {
            source: kuser.faceIconUrl
            anchors.centerIn: parent
            sourceSize.width: 50
            sourceSize.height: 50
        }
    }
    Item {
        width: grid.width/3
        height: 50
        Button{
            text: i18nd("kcm_pulseaudio", "Side Right")
            anchors.centerIn: parent
            visible: sinkObject ? sinkObject.rawChannels.indexOf("side-right") > -1 : false
            onClicked: sinkObject.testChannel("side-right")
        }
    }
    Item {
        width: grid.width/3
        height: 50
        Button{
            text: i18nd("kcm_pulseaudio", "Rear Left")
            anchors.centerIn: parent
            visible: sinkObject ? sinkObject.rawChannels.indexOf("rear-left") > -1 : false
            onClicked: sinkObject.testChannel("rear-left")
        }
    }
    Item {
        width: grid.width/3
        height: 50
        Button{
            text: i18nd("kcm_pulseaudio", "Subwoofer")
            anchors.centerIn: parent
            visible: sinkObject ? sinkObject.rawChannels.indexOf("lfe") > -1 : false
            // there is no subwoofer sound in the freedesktop theme https://gitlab.freedesktop.org/xdg/xdg-sound-theme/-/issues/7
            onClicked: sinkObject.testChannel("rear-center")
        }
    }
    Item {
        width: grid.width/3
        height: 50
        Button{
            text: i18nd("kcm_pulseaudio", "Rear Right")
            anchors.centerIn: parent
            visible: sinkObject ? sinkObject.rawChannels.indexOf("rear-right") > -1 : false
            onClicked: sinkObject.testChannel("rear-right")
        }
    }
}
