/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>
    SPDX-FileCopyrightText: 2019 Sefa Eyeoglu <contact@scrumplex.net>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick 2.4
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.5 as QQC2
import org.kde.kirigami 2.5 as Kirigami

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
                return Kirigami.Theme.textColor
            } else if (displayValue > 100 && displayValue <= 125) {
                return Kirigami.Theme.neutralTextColor
            } else {
                return Kirigami.Theme.negativeTextColor
            }
        }
    }

    TextMetrics {
        id: percentMetrics
        font: percentText.font
        text: i18ndc("kcm_pulseaudio", "only used for sizing, should be widest possible string", "100%")
    }
}
