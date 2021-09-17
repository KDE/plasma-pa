/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>
    SPDX-FileCopyrightText: 2019 Sefa Eyeoglu <contact@scrumplex.net>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick 2.4
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

import org.kde.kquickcontrolsaddons 2.0
import org.kde.plasma.components 2.0 as PlasmaComponents // for ListItem
import org.kde.plasma.components 3.0 as PlasmaComponents3
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.extras 2.0 as PlasmaExtras
import org.kde.draganddrop 2.0 as DragAndDrop
import org.kde.plasma.private.volume 0.1

import "../code/icon.js" as Icon

PlasmaComponents.ListItem {
    id: item

    property alias label: defaultButton.text
    property alias draggable: dragArea.enabled
    property alias icon: clientIcon.source
    property alias iconUsesPlasmaTheme: clientIcon.usesPlasmaTheme
    property string type
    property string fullNameToShowOnHover: ""

    checked: dropArea.containsDrag
    opacity: (draggedStream && draggedStream.deviceIndex == Index) ? 0.3 : 1.0
    separatorVisible: false

    ListView.delayRemove: dragArea.dragActive

    Item {
        width: parent.width
        height: column.height

        RowLayout {
            id: controlsRow
            spacing: PlasmaCore.Units.smallSpacing
            anchors.left: parent.left
            anchors.right: parent.right

            PlasmaCore.IconItem {
                id: clientIcon
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                Layout.preferredHeight: column.height * 0.75
                Layout.preferredWidth: Layout.preferredHeight
                source: "unknown"
                visible: type === "sink-input" || type === "source-output"

                onSourceChanged: {
                    if (!valid && source != "unknown") {
                        source = "unknown";
                    }
                }

                PlasmaCore.IconItem {
                    anchors {
                        right: parent.right
                        bottom: parent.bottom
                        margins: PlasmaCore.Units.smallSpacing
                    }
                    width: PlasmaCore.Units.iconSizes.small
                    height: width
                    source: item.type === "sink-input" || item.type === "source-output" ? "emblem-pause" : ""
                    visible: valid && Corked

                    PlasmaComponents3.ToolTip {
                        visible: parent.visible && dragMouseArea.containsMouse
                        text: item.type === "source-output" ? i18n("Currently not recording")
                                                            : i18n("Currently not playing")
                    }
                }

                DragAndDrop.DragArea {
                    id: dragArea
                    anchors.fill: parent
                    delegate: parent

                    mimeData {
                        source: item
                    }

                    onDragStarted: {
                        draggedStream = model.PulseObject;
                        beginMoveStream(type == "sink-input" ? "sink" : "source");
                    }

                    onDrop: {
                        draggedStream = null;
                        endMoveStream();
                    }

                    MouseArea {
                        id: dragMouseArea
                        anchors.fill: parent
                        cursorShape: dragArea.enabled ? (pressed && pressedButtons === Qt.LeftButton ? Qt.ClosedHandCursor : Qt.OpenHandCursor) : undefined
                        acceptedButtons: Qt.LeftButton | Qt.MiddleButton
                        hoverEnabled: true
                        onClicked: {
                            if (mouse.button === Qt.MiddleButton) {
                                Muted = !Muted;
                            }
                        }
                    }
                }
            }

            ColumnLayout {
                id: column
                spacing: 0

                RowLayout {
                    Layout.minimumHeight: contextMenuButton.implicitHeight

                    PlasmaComponents3.RadioButton {
                        id: defaultButton
                        // Maximum width of the button need to match the text. Empty area must not change the default device.
                        Layout.maximumWidth: controlsRow.width - Layout.leftMargin - Layout.rightMargin
                                              - (contextMenuButton.visible ? contextMenuButton.implicitWidth + PlasmaCore.Units.smallSpacing * 2 : 0)
                        Layout.leftMargin: LayoutMirroring.enabled ? 0 : Math.round((muteButton.width - defaultButton.indicator.width) / 2)
                        Layout.rightMargin: LayoutMirroring.enabled ? Math.round((muteButton.width - defaultButton.indicator.width) / 2) : 0
                        spacing: PlasmaCore.Units.smallSpacing + Math.round((muteButton.width - defaultButton.indicator.width) / 2)
                        checked: model.PulseObject.hasOwnProperty("default") ? model.PulseObject.default : false
                        visible: (type == "sink" && sinkView.model.count > 1) || (type == "source" && sourceView.model.count > 1)
                        onClicked: model.PulseObject.default = true;
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        visible: !defaultButton.visible

                        // User-friendly name
                        PlasmaComponents3.Label {
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
                        PlasmaComponents3.Label {
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

                        PlasmaComponents3.ToolTip {
                            text: i18n("Show additional options for %1", defaultButton.text)
                        }
                    }
                }

                RowLayout {
                    SmallToolButton {
                        id: muteButton
                        readonly property bool isPlayback: type.substring(0, 4) == "sink"
                        icon.name: Icon.name(Volume, Muted, isPlayback ? "audio-volume" : "microphone-sensitivity")
                        onClicked: Muted = !Muted
                        checked: Muted

                        PlasmaComponents3.ToolTip {
                            text: i18n("Mute %1", defaultButton.text)
                        }
                    }

                    PlasmaComponents3.Slider {
                        id: slider

                        // Helper properties to allow async slider updates.
                        // While we are sliding we must not react to value updates
                        // as otherwise we can easily end up in a loop where value
                        // changes trigger volume changes trigger value changes.
                        property int volume: Volume
                        property bool ignoreValueChange: true
                        readonly property bool forceRaiseMaxVolume: (raiseMaximumVolumeCheckbox.checked && (type === "sink" || type === "source"))
                                                                    || volume >= PulseAudio.NormalVolume * 1.01

                        Layout.fillWidth: true
                        from: PulseAudio.MinimalVolume
                        to: forceRaiseMaxVolume ? PulseAudio.MaximalVolume : PulseAudio.NormalVolume
                        stepSize: to / (to / PulseAudio.NormalVolume * 100.0)
                        visible: HasVolume
                        enabled: VolumeWritable
                        opacity: Muted ? 0.5 : 1

                        Accessible.name: i18nc("Accessibility data on volume slider", "Adjust volume for %1", defaultButton.text)

                        handle.clip: true // prevents a visual glitch, BUG 434927

                        background:  PlasmaCore.FrameSvgItem {
                            imagePath: "widgets/slider"
                            prefix: "groove"
                            width: parent.availableWidth
                            height: margins.top + margins.bottom
                            anchors.centerIn: parent
                            scale: parent.mirrored ? -1 : 1

                            PlasmaCore.FrameSvgItem {
                                imagePath: "widgets/slider"
                                prefix: "groove-highlight"
                                anchors.left: parent.left
                                y: (parent.height - height) / 2
                                width: Math.max(margins.left + margins.right, slider.handle.x * meter.volume)
                                height: Math.max(margins.top + margins.bottom, parent.height)
                                opacity: meter.available && (meter.volume > 0 || animation.running)
                                clip: true // prevents a visual glitch, BUG 434927
                                VolumeMonitor {
                                    id: meter
                                    target: slider.visible && model.PulseObject ? model.PulseObject : null
                                }
                                Behavior on width {
                                    NumberAnimation  {
                                        id: animation
                                        duration: PlasmaCore.Units.shortDuration
                                        easing.type: Easing.OutQuad
                                    }
                                }
                            }
                        }

                        Component.onCompleted: {
                            ignoreValueChange = false;
                        }

                        onVolumeChanged: {
                            var oldIgnoreValueChange = ignoreValueChange;
                            ignoreValueChange = true;
                            value = Volume;
                            ignoreValueChange = oldIgnoreValueChange;
                        }

                        onValueChanged: {
                            if (!ignoreValueChange) {
                                Volume = value;
                                Muted = value == 0;

                                if (!pressed) {
                                    updateTimer.restart();
                                }
                            }
                        }

                        onPressedChanged: {
                            if (!pressed) {
                                // Make sure to sync the volume once the button was
                                // released.
                                // Otherwise it might be that the slider is at v10
                                // whereas PA rejected the volume change and is
                                // still at v15 (e.g.).
                                updateTimer.restart();

                                if (type == "sink") {
                                    playFeedback(Index);
                                }
                            }
                        }

                        Timer {
                            id: updateTimer
                            interval: 200
                            onTriggered: slider.value = Volume
                        }
                    }
                    PlasmaComponents3.Label {
                        id: percentText
                        readonly property real value: model.PulseObject.volume > slider.to ? model.PulseObject.volume : slider.value
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

        DragAndDrop.DropArea {
            id: dropArea
            anchors.fill: parent
            enabled: draggedStream

            onDragEnter: {
                if (draggedStream.deviceIndex == Index) {
                    event.ignore();
                }
            }

            onDrop: {
                draggedStream.deviceIndex = Index;
            }
        }

        MouseArea {
            anchors {
                fill: parent
                leftMargin: clientIcon.width
            }
            acceptedButtons: Qt.MiddleButton | Qt.RightButton
            onPressed: {
                if (mouse.button === Qt.RightButton) {
                    contextMenu.visualParent = this;
                    contextMenu.open(mouse.x, mouse.y);
                }
            }
            onClicked: {
                if (mouse.button === Qt.MiddleButton) {
                    Muted = !Muted;
                }
            }
        }
    }

    ListItemMenu {
        id: contextMenu
        pulseObject: model.PulseObject
        cardModel: paCardModel
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
        sourceModel: {
            if (item.type.includes("sink")) {
                return sinkView.model;
            } else if (item.type.includes("source")) {
                return sourceView.model;
            }
        }
    }

    function setVolumeByPercent(targetPercent) {
        model.PulseObject.volume = Math.round(PulseAudio.NormalVolume * (targetPercent/100));
    }

    Keys.onPressed: {
        if (event.key == Qt.Key_M) {
            muteButton.clicked();
        } else if (event.key == Qt.Key_0) {
            setVolumeByPercent(0);
        } else if (event.key == Qt.Key_1) {
            setVolumeByPercent(10);
        } else if (event.key == Qt.Key_2) {
            setVolumeByPercent(20);
        } else if (event.key == Qt.Key_3) {
            setVolumeByPercent(30);
        } else if (event.key == Qt.Key_4) {
            setVolumeByPercent(40);
        } else if (event.key == Qt.Key_5) {
            setVolumeByPercent(50);
        } else if (event.key == Qt.Key_6) {
            setVolumeByPercent(60);
        } else if (event.key == Qt.Key_7) {
            setVolumeByPercent(70);
        } else if (event.key == Qt.Key_8) {
            setVolumeByPercent(80);
        } else if (event.key == Qt.Key_9) {
            setVolumeByPercent(90);
        } else if (event.key == Qt.Key_Return) {
            if (defaultButton.visible) {
                defaultButton.clicked();
            }
        } else if (event.key == Qt.Key_Menu) {
            contextMenuButton.clicked();
        } else {
            return; // don't accept the key press
        }
        event.accepted = true;
    }
}
