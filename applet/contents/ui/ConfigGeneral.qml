/*
    Copyright 2016 David Rosca <nowrep@gmail.com>

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

import QtQuick 2.5
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5 as QQC2

import org.kde.kirigami 2.5 as Kirigami

import org.kde.plasma.private.volume 0.1

Kirigami.FormLayout {
    property alias cfg_maximumVolume: maximumVolume.value
    property alias cfg_volumeStep: volumeStep.value
    property alias cfg_volumeFeedback: volumeFeedback.checked
    property alias cfg_outputChangeOsd: outputChangeOsd.checked

    VolumeFeedback {
        id: feedback
    }


    QQC2.SpinBox {
        id: maximumVolume

        Kirigami.FormData.label: i18n("Maximum volume:")

        from: 10
        to: 150
        stepSize: 1
        textFromValue: function(value) {
            return value + "%";
        }
    }

    QQC2.SpinBox {
        id: volumeStep
        implicitWidth: maximumVolume.width

        Kirigami.FormData.label: i18n("Volume step:")

        from: 1
        to: 100
        stepSize: 1
        textFromValue: function(value) {
            return value + "%";
        }
    }


    Item {
        Kirigami.FormData.isSection: true
    }


    QQC2.CheckBox {
        id: volumeFeedback

        Kirigami.FormData.label: i18n("Feedback:")

        text: i18n("Play sound when volume changes")
        enabled: feedback.valid
    }

    QQC2.CheckBox {
        id: outputChangeOsd
        text: i18n("Display notification when default output device changes")
    }
}
