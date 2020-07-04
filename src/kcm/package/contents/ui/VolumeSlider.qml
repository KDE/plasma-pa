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

import QtQuick 2.4
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.5 as QQC2

import org.kde.plasma.private.volume 0.1

RowLayout {
    id: sliderRow

    Layout.bottomMargin: hundredPercentLabelVisible ? hundredPercentLabel.height : 0

    signal moved()

    property alias value: slider.value

    property bool hundredPercentLabelVisible: true

    QQC2.Slider {
        id: slider

        Layout.fillWidth: true

        value: Volume
        from: PulseAudio.MinimalVolume
        to: PulseAudio.MaximalVolume
        // TODO: implement a way to hide tickmarks
        // stepSize: to / (PulseAudio.MaximalVolume / PulseAudio.NormalVolume * 100.0) 
        visible: HasVolume
        enabled: VolumeWritable
        opacity: Muted ? 0.5 : 1
        onMoved: sliderRow.moved()

        QQC2.Label {
            id: hundredPercentLabel
            readonly property real hundredPos: (slider.width / slider.to) * PulseAudio.NormalVolume
            z: slider.z - 1
            x: (Qt.application.layoutDirection == Qt.RightToLeft ? slider.width - hundredPos : hundredPos) - width / 2
            y: slider.height
            visible: sliderRow.hundredPercentLabelVisible
            opacity: 0.5
            font.pixelSize: slider.height / 2.2
            text: i18nd("kcm_pulseaudio", "100%")
        }
    }

    QQC2.Label {
        id: percentText
        readonly property real value: PulseObject.volume > slider.maximumValue ? PulseObject.volume : slider.value
        readonly property real displayValue: Math.round(value / PulseAudio.NormalVolume * 100.0)
        Layout.alignment: Qt.AlignHCenter
        Layout.minimumWidth: percentMetrics.advanceWidth
        horizontalAlignment: Qt.AlignRight
        text: i18ndc("kcm_pulseaudio", "volume percentage", "%1%", displayValue)
        // Display a subtle visual indication that the volume might be
        // dangerously high
        // ------------------------------------------------
        // Keep this in sync with the copies in ListItemBase.qml
        // and plasma-workspace:OSDItem.qml
        color: {
            if (displayValue <= 100) {
                return theme.textColor
            } else if (displayValue > 100 && displayValue <= 125) {
                return theme.neutralTextColor
            } else {
                return theme.negativeTextColor
            }
        }
    }

    TextMetrics {
        id: percentMetrics
        font: percentText.font
        text: i18ndc("kcm_pulseaudio", "only used for sizing, should be widest possible string", "100%")
    }
}
