/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>
    SPDX-FileCopyrightText: 2019 Sefa Eyeoglu <contact@scrumplex.net>
    SPDX-FileCopyrightText: 2022 ivan (@ratijas) tkachenko <me@ratijas.tk>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/
import QtQuick 2.15
import QtQuick.Layouts 1.15

import org.kde.kquickcontrolsaddons 2.0
import org.kde.plasma.components 3.0 as PC3
import org.kde.ksvg 1.0 as KSvg
import org.kde.kirigami 2.20 as Kirigami
import org.kde.plasma.private.volume 0.1

// Audio volume slider. Value represents desired volume level in
// device-specific units, while volume property reports current volume level
// normalized to 0..1 range.
PC3.Slider {
    id: control

    property VolumeObject volumeObject

    // When muted, the whole slider will appear slightly faded, but remain
    // functional and interactive.
    property bool muted: false

    // Current (monitored) volume. To be animated. Do not update too fast
    // (i.e. faster or close to screen refresh rate), otherwise it won't
    // animate smoothly.
    property real volume: meter.volume

    VolumeMonitor {
        id: meter
        target: control.visible ? control.volumeObject : null
    }

    Behavior on volume {
        NumberAnimation  {
            id: animate
            duration: Kirigami.Units.shortDuration
            easing.type: Easing.OutQuad
        }
    }

    // When a maximum volume limit is raised/lower, animate the change.
    Behavior on to {
        NumberAnimation {
            duration: Kirigami.Units.shortDuration
            easing.type: Easing.InOutQuad
        }
    }

    opacity: muted ? 0.5 : 1
    // Prevents the groove from showing through the handle
    layer.enabled: opacity < 1

    background: KSvg.FrameSvgItem {
        imagePath: "widgets/slider"
        prefix: "groove"

        implicitWidth: control.horizontal ? Kirigami.Units.gridUnit * 12 : fixedMargins.left + fixedMargins.right
        implicitHeight: control.vertical ? Kirigami.Units.gridUnit * 12 : fixedMargins.top + fixedMargins.bottom

        width: control.horizontal ? Math.max(fixedMargins.left + fixedMargins.right, control.availableWidth) : implicitWidth
        height: control.vertical ? Math.max(fixedMargins.top + fixedMargins.bottom, control.availableHeight) : implicitHeight

        x: control.leftPadding + (control.horizontal ? 0 : Math.round((control.availableWidth - width) / 2))
        y: control.topPadding + (control.vertical ? 0 : Math.round((control.availableHeight - height) / 2))

        KSvg.FrameSvgItem {
            imagePath: "widgets/slider"
            prefix: "groove-highlight"

            anchors.left: parent.left
            anchors.bottom: parent.bottom
            LayoutMirroring.enabled: control.mirrored

            width: control.horizontal ? Math.max(fixedMargins.left + fixedMargins.right, Math.round(control.position * (control.availableWidth - control.handle.width / 2) + (control.handle.width / 2))) : parent.width
            height: control.vertical ? Math.max(fixedMargins.top + fixedMargins.bottom, Math.round(control.position * (control.availableHeight - control.handle.height / 2) + (control.handle.height / 2))) : parent.height
        }

        KSvg.FrameSvgItem {
            imagePath: "widgets/slider"
            prefix: "groove-highlight"
            status: KSvg.FrameSvgItem.Selected
            visible: meter.available && control.volume > 0

            anchors.left: parent.left
            anchors.bottom: parent.bottom
            LayoutMirroring.enabled: control.mirrored

            width: control.horizontal ? Math.max(fixedMargins.left + fixedMargins.right, Math.round(control.volume * control.position * control.availableWidth)) : parent.width
            height: control.vertical ? Math.max(fixedMargins.top + fixedMargins.bottom, Math.round(control.volume * control.position * control.availableHeight)) : parent.height
        }
    }
}
