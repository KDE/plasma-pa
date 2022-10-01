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
import org.kde.plasma.core 2.1 as PlasmaCore
import org.kde.plasma.extras 2.0 as PlasmaExtras
import org.kde.plasma.private.volume 0.1

import "../code/icon.js" as Icon

PC3.ItemDelegate {
    id: item

    required property var model
    property alias label: defaultButton.text
    property alias draggable: dragMouseArea.enabled
    property alias iconSource: clientIcon.source
    property alias iconUsesPlasmaTheme: clientIcon.usesPlasmaTheme
    // TODO: convert to a proper enum?
    property string /* "sink" | "sink-input" | "source" | "source-output" */ type
    property string fullNameToShowOnHover: ""

    highlighted: dropArea.containsDrag || activeFocus
    background.visible: highlighted
    opacity: (plasmoid.rootItem.draggedStream && plasmoid.rootItem.draggedStream.deviceIndex === item.model.Index) ? 0.3 : 1.0

    ListView.delayRemove: clientIcon.Drag.active

    Keys.forwardTo: [slider]

    contentItem: RowLayout {
        id: controlsRow
        spacing: item.spacing

        PlasmaCore.IconItem {
            id: clientIcon
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            implicitHeight: PlasmaCore.Units.iconSizes.medium
            implicitWidth: implicitHeight
            source: "unknown"
            visible: item.type === "sink-input" || item.type === "source-output"

            onSourceChanged: {
                if (!valid && source !== "unknown") {
                    source = "unknown";
                }
            }

            PlasmaCore.IconItem {
                anchors {
                    right: parent.right
                    bottom: parent.bottom
                }
                implicitHeight: PlasmaCore.Units.iconSizes.small
                implicitWidth: implicitHeight
                source: item.type === "sink-input" || item.type === "source-output" ? "emblem-pause" : ""
                visible: valid && item.model.Corked

                PC3.ToolTip.visible: visible && dragMouseArea.containsMouse
                PC3.ToolTip.text: item.type === "source-output"
                    ? i18n("Currently not recording")
                    : i18n("Currently not playing")
                PC3.ToolTip.delay: 700
            }

            MouseArea {
                id: dragMouseArea
                enabled: contextMenu.status === 3 //Closed
                anchors.fill: parent
                cursorShape: enabled ? (pressed && pressedButtons === Qt.LeftButton ? Qt.ClosedHandCursor : Qt.OpenHandCursor) : undefined
                acceptedButtons: Qt.LeftButton | Qt.MiddleButton
                hoverEnabled: true
                drag.target: clientIcon
                onClicked: if (mouse.button === Qt.MiddleButton) {
                    item.model.Muted = !item.model.Muted;
                }
                onPressed: if (mouse.button === Qt.LeftButton) {
                    clientIcon.grabToImage(result => {
                        clientIcon.Drag.imageSource = result.url;
                    });
                }
            }
            Drag.active: dragMouseArea.drag.active
            Drag.dragType: Drag.Automatic
            Drag.onDragStarted: {
                plasmoid.rootItem.draggedStream = item.model.PulseObject;
                beginMoveStream(item.type === "sink-input" ? "sink" : "source");
            }
            Drag.onDragFinished: {
                plasmoid.rootItem.draggedStream = null;
                endMoveStream();
            }
        }

        ColumnLayout {
            id: column
            spacing: 0

            RowLayout {
                Layout.minimumHeight: contextMenuButton.implicitHeight

                PC3.RadioButton {
                    id: defaultButton
                    // Maximum width of the button need to match the text. Empty area must not change the default device.
                    Layout.maximumWidth: controlsRow.width - Layout.leftMargin - Layout.rightMargin
                                            - (contextMenuButton.visible ? contextMenuButton.implicitWidth + PlasmaCore.Units.smallSpacing * 2 : 0)
                    Layout.leftMargin: LayoutMirroring.enabled ? 0 : Math.round((muteButton.width - defaultButton.indicator.width) / 2)
                    Layout.rightMargin: LayoutMirroring.enabled ? Math.round((muteButton.width - defaultButton.indicator.width) / 2) : 0
                    spacing: PlasmaCore.Units.smallSpacing + Math.round((muteButton.width - defaultButton.indicator.width) / 2)
                    checked: item.model.PulseObject.hasOwnProperty("default") ? item.model.PulseObject.default : false
                    visible: (item.type === "sink" || item.type === "source") && item.ListView.view.count > 1
                    onClicked: item.model.PulseObject.default = true;
                }

                RowLayout {
                    Layout.fillWidth: true
                    visible: !defaultButton.visible

                    // User-friendly name
                    PC3.Label {
                        Layout.fillWidth: !longDescription.visible
                        text: defaultButton.text
                        elide: Text.ElideRight

                        MouseArea {
                            id: labelHoverHandler

                            // Only want to handle hover for the width of
                            // the actual text item itself
                            anchors.left: parent.left
                            anchors.top: parent.top
                            width: parent.contentWidth
                            height: parent.contentHeight

                            enabled: item.fullNameToShowOnHover.length > 0
                            hoverEnabled: true
                            acceptedButtons: Qt.NoButton
                        }
                    }
                    // Possibly not user-friendly description; only show on hover
                    PC3.Label {
                        id: longDescription

                        Layout.fillWidth: true
                        visible: opacity > 0
                        opacity: labelHoverHandler.containsMouse ? 1 : 0
                        Behavior on opacity {
                            NumberAnimation {
                                duration: PlasmaCore.Units.shortDuration
                                easing.type: Easing.InOutQuad
                            }
                        }

                        // Not a word puzzle because this is not a translated string
                        text: "(" + item.fullNameToShowOnHover + ")"
                        elide: Text.ElideRight
                    }
                }

                Item {
                    Layout.fillWidth: true
                    visible: contextMenuButton.visible
                }

                SmallToolButton {
                    id: contextMenuButton
                    icon.name: "application-menu"
                    checked: contextMenu.visible && contextMenu.visualParent === this
                    onPressed: {
                        contextMenu.visualParent = this;
                        contextMenu.openRelative();
                    }
                    visible: contextMenu.hasContent

                    text: i18nc("@action:button", "Additional Options")

                    Accessible.description: i18n("Show additional options for %1", defaultButton.text)
                    Accessible.role: Accessible.ButtonMenu

                    PC3.ToolTip {
                        text: parent.Accessible.description
                    }
                }
            }

            RowLayout {
                SmallToolButton {
                    id: muteButton
                    readonly property bool isPlayback: item.type.startsWith("sink")
                    icon.name: Icon.name(item.model.Volume, item.model.Muted, isPlayback ? "audio-volume" : "microphone-sensitivity")
                    onClicked: item.model.Muted = !item.model.Muted
                    checked: item.model.Muted

                    text: item.model.Muted ? i18nc("@action:button", "Unmute") : i18nc("@action:button", "Mute")

                    Accessible.description: item.model.Muted ? i18n("Unmute %1", defaultButton.text) : i18n("Mute %1", defaultButton.text)

                    PC3.ToolTip {
                        text: parent.Accessible.description
                    }
                }

                VolumeSlider {
                    id: slider

                    readonly property bool forceRaiseMaxVolume: (raiseMaximumVolumeCheckbox.checked && (item.type === "sink" || item.type === "source"))

                    Layout.fillWidth: true
                    from: PulseAudio.MinimalVolume
                    to: forceRaiseMaxVolume || item.model.Volume >= PulseAudio.NormalVolume * 1.01 ? PulseAudio.MaximalVolume : PulseAudio.NormalVolume
                    stepSize: to / (to / PulseAudio.NormalVolume * 100.0)
                    visible: item.model.HasVolume
                    enabled: item.model.VolumeWritable
                    muted: item.model.Muted
                    volumeObject: item.model.PulseObject
                    Accessible.name: i18nc("Accessibility data on volume slider", "Adjust volume for %1", defaultButton.text)

                    value: item.model.Volume
                    onMoved: {
                        item.model.Volume = value;
                        item.model.Muted = value === 0;
                    }
                    onPressedChanged: {
                        if (!pressed) {
                            // Make sure to sync the volume once the button was
                            // released.
                            // Otherwise it might be that the slider is at v10
                            // whereas PA rejected the volume change and is
                            // still at v15 (e.g.).
                            value = Qt.binding(() => item.model.Volume);
                            if (type === "sink") {
                                playFeedback(item.model.Index);
                            }
                        }
                    }
                    onForceRaiseMaxVolumeChanged: {
                        if (forceRaiseMaxVolume) {
                            toAnimation.from = PulseAudio.NormalVolume;
                            toAnimation.to = PulseAudio.MaximalVolume;
                        } else {
                            toAnimation.from = PulseAudio.MaximalVolume;
                            toAnimation.to = PulseAudio.NormalVolume;
                        }
                        seqAnimation.restart();
                    }

                    function updateVolume() {
                        if (!forceRaiseMaxVolume && item.model.Volume > PulseAudio.NormalVolume) {
                            item.model.Volume = PulseAudio.NormalVolume;
                        }
                    }

                    SequentialAnimation {
                        id: seqAnimation
                        NumberAnimation {
                            id: toAnimation
                            target: slider
                            property: "to"
                            duration: PlasmaCore.Units.shortDuration
                            easing.type: Easing.InOutQuad
                        }
                        ScriptAction {
                            script: slider.updateVolume()
                        }
                    }
                }
                PC3.Label {
                    id: percentText
                    readonly property real value: item.model.PulseObject.volume > slider.to ? item.model.PulseObject.volume : slider.value
                    readonly property real displayValue: Math.round(value / PulseAudio.NormalVolume * 100.0)
                    Layout.alignment: Qt.AlignHCenter
                    Layout.minimumWidth: percentMetrics.advanceWidth
                    horizontalAlignment: Qt.AlignRight
                    text: i18nc("volume percentage", "%1%", displayValue)
                    // Display a subtle visual indication that the volume
                    // might be dangerously high
                    // ------------------------------------------------
                    // Keep this in sync with the copies in VolumeSlider.qml
                    // and plasma-workspace:OSDItem.qml
                    color: {
                        if (displayValue <= 100) {
                            return PlasmaCore.Theme.textColor
                        } else if (displayValue > 100 && displayValue <= 125) {
                            return PlasmaCore.Theme.neutralTextColor
                        } else {
                            return PlasmaCore.Theme.negativeTextColor
                        }
                    }
                }

                TextMetrics {
                    id: percentMetrics
                    font: percentText.font
                    text: i18nc("only used for sizing, should be widest possible string", "100%")
                }
            }
        }
    }

    MouseArea {
        z: -1
        parent: item
        anchors.fill: parent
        acceptedButtons: Qt.MiddleButton | Qt.RightButton
        onPressed: {
            if (mouse.button === Qt.RightButton) {
                contextMenu.visualParent = this;
                contextMenu.open(mouse.x, mouse.y);
            }
        }
        onClicked: {
            if (mouse.button === Qt.MiddleButton) {
                item.model.Muted = !item.model.Muted;
            }
        }
    }

    DropArea {
        id: dropArea
        z: -1
        parent: item
        anchors.fill: parent
        enabled: plasmoid.rootItem.draggedStream && plasmoid.rootItem.draggedStream.deviceIndex !== item.model.Index
        onDropped: {
            plasmoid.rootItem.draggedStream.deviceIndex = item.model.Index;
        }
    }

    ListItemMenu {
        id: contextMenu
        pulseObject: model.PulseObject
        cardModel: plasmoid.rootItem.paCardModel
        itemType: {
            switch (item.type) {
            case "sink":
                return ListItemMenu.Sink;
            case "sink-input":
                return ListItemMenu.SinkInput;
            case "source":
                return ListItemMenu.Source;
            case "source-output":
                return ListItemMenu.SourceOutput;
            }
        }
        sourceModel: if (item.type.startsWith("sink")) {
            return plasmoid.rootItem.paSinkFilterModel
        }  else if (item.type.startsWith("source")) {
            return plasmoid.rootItem.paSourceFilterModel
        }
    }

    function setVolumeByPercent(targetPercent) {
        item.model.PulseObject.volume = Math.round(PulseAudio.NormalVolume * (targetPercent/100));
    }

    Keys.onPressed: {
        const k = event.key;

        if (k === Qt.Key_M) {
            muteButton.clicked();
        } else if (k >= Qt.Key_0 && k <= Qt.Key_9) {
            setVolumeByPercent((k - Qt.Key_0) * 10);
        } else if (k === Qt.Key_Return) {
            if (defaultButton.visible) {
                defaultButton.clicked();
            }
        } else if (k === Qt.Key_Menu) {
            contextMenu.visualParent = contextMenuButton;
            contextMenu.openRelative();
        } else {
            return; // don't accept the key press
        }
        event.accepted = true;
    }
}
