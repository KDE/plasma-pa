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

import QtQuick 2.4
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

import org.kde.kquickcontrolsaddons 2.0
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.extras 2.0 as PlasmaExtras
import org.kde.draganddrop 2.0 as DragAndDrop
import org.kde.plasma.private.volume 0.1

import "../code/icon.js" as Icon

PlasmaComponents.ListItem {
    id: item

    property alias label: textLabel.text
    property alias draggable: dragArea.enabled
    property alias icon: clientIcon.source
    property string type

    checked: dropArea.containsDrag
    opacity: (draggedStream && draggedStream.deviceIndex == Index) ? 0.3 : 1.0

    ListView.delayRemove: dragArea.dragActive

    Item {
        width: parent.width
        height: rowLayout.height

        RowLayout {
            id: rowLayout
            width: parent.width
            spacing: units.smallSpacing

            PlasmaCore.IconItem {
                id: clientIcon
                Layout.alignment: Qt.AlignHCenter
                Layout.preferredHeight: column.height * 0.75
                Layout.preferredWidth: Layout.preferredHeight
                source: "unknown"

                onSourceChanged: {
                    if (!valid && source != "unknown") {
                        source = "unknown";
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
                        draggedStream = PulseObject;
                        beginMoveStream(type == "sink-input" ? "sink" : "source");
                    }

                    onDrop: {
                        draggedStream = null;
                        endMoveStream();
                    }

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: dragArea.enabled ? (pressed && pressedButtons === Qt.LeftButton ? Qt.ClosedHandCursor : Qt.OpenHandCursor) : undefined
                        acceptedButtons: Qt.LeftButton | Qt.MiddleButton
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
                spacing: 1

                RowLayout {
                    Layout.fillWidth: true

                    PlasmaExtras.Heading {
                        id: textLabel
                        Layout.fillWidth: true
                        height: undefined
                        level: 5
                        opacity: 0.6
                        wrapMode: Text.NoWrap
                        elide: Text.ElideRight
                    }
                    SmallToolButton {
                        id: contextMenuButton
                        icon: "application-menu"
                        checkable: true
                        onClicked: contextMenu.show()
                        tooltip: i18n("Show additional options for %1", textLabel.text)
                    }
                }

                RowLayout {
                    SmallToolButton {
                        readonly property bool isPlayback: type.substring(0, 4) == "sink"
                        icon: Icon.name(Volume, Muted, isPlayback ? "audio-volume" : "microphone-sensitivity")
                        onClicked: Muted = !Muted
                        tooltip: i18n("Mute %1", textLabel.text)

                    }

                    PlasmaComponents.Slider {
                        id: slider

                        // Helper properties to allow async slider updates.
                        // While we are sliding we must not react to value updates
                        // as otherwise we can easily end up in a loop where value
                        // changes trigger volume changes trigger value changes.
                        property int volume: Volume
                        property bool ignoreValueChange: true
                        property bool forceRaiseMaxVolume: false
                        readonly property bool raiseMaxVolume: forceRaiseMaxVolume || volume >= PulseAudio.NormalVolume * 1.01

                        Layout.fillWidth: true
                        minimumValue: PulseAudio.MinimalVolume
                        maximumValue: raiseMaxVolume ? PulseAudio.MaximalVolume : maxVolumeValue
                        stepSize: maximumValue / (maximumValue / PulseAudio.NormalVolume * 100.0)
                        visible: HasVolume
                        enabled: VolumeWritable
                        opacity: Muted ? 0.5 : 1

                        Accessible.name: i18nc("Accessibility data on volume slider", "Adjust volume for %1", textLabel.text)

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

                                if (type == "sink") {
                                    playFeedback(Index);
                                }

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
                            }
                        }

                        Timer {
                            id: updateTimer
                            interval: 200
                            onTriggered: slider.value = Volume
                        }
                    }
                    PlasmaComponents.Label {
                        id: percentText
                        readonly property real value: PulseObject.volume > slider.maximumValue ? PulseObject.volume : slider.value
                        Layout.alignment: Qt.AlignHCenter
                        Layout.minimumWidth: percentMetrics.advanceWidth
                        horizontalAlignment: Qt.AlignRight
                        text: i18nc("volume percentage", "%1%", Math.round(value / PulseAudio.NormalVolume * 100.0))
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
            acceptedButtons: Qt.MiddleButton
            onClicked: Muted = !Muted
        }
    }

    PlasmaComponents.ContextMenu {
        id: contextMenu

        visualParent: contextMenuButton
        placement: PlasmaCore.Types.BottomPosedLeftAlignedPopup

        onStatusChanged: {
            if (status == PlasmaComponents.DialogStatus.Closed) {
                contextMenuButton.checked = false;
            }
        }

        function newSeperator() {
            return Qt.createQmlObject("import org.kde.plasma.components 2.0 as PlasmaComponents; PlasmaComponents.MenuItem { separator: true }", contextMenu);
        }
        function newMenuItem() {
            return Qt.createQmlObject("import org.kde.plasma.components 2.0 as PlasmaComponents; PlasmaComponents.MenuItem {}", contextMenu);
        }

        function loadDynamicActions() {
            contextMenu.clearMenuItems();

            // Mute
            var menuItem = newMenuItem();
            menuItem.text = i18nc("Checkable switch for (un-)muting sound output.", "Mute");
            menuItem.checkable = true;
            menuItem.checked = Muted;
            menuItem.clicked.connect(function() {
                Muted = !Muted
            });
            contextMenu.addMenuItem(menuItem);

            // Default
            if (typeof PulseObject.default === "boolean") {
                var menuItem = newMenuItem();
                menuItem.text = i18nc("Checkable switch to change the current default output.", "Default");
                menuItem.checkable = true;
                menuItem.checked = PulseObject.default
                menuItem.clicked.connect(function() {
                    PulseObject.default = true
                });
                contextMenu.addMenuItem(menuItem);
            }

            // Raise max volume
            menuItem = newMenuItem();
            menuItem.text = i18n("Raise maximum volume");
            menuItem.checkable = true;
            menuItem.checked = slider.forceRaiseMaxVolume;
            menuItem.clicked.connect(function() {
                slider.forceRaiseMaxVolume = !slider.forceRaiseMaxVolume;
                if (!slider.forceRaiseMaxVolume && Volume > PulseAudio.NormalVolume) {
                    Volume = PulseAudio.NormalVolume;
                }
            });
            contextMenu.addMenuItem(menuItem);

            // Ports
            if (PulseObject.ports && PulseObject.ports.length > 0) {
                contextMenu.addMenuItem(newSeperator());

                var isMultiplePorts = (1 < PulseObject.ports.length);
                var menuItem = newMenuItem();
                menuItem.text = i18nc("Heading for a list of ports of a device (for example built-in laptop speakers or a plug for headphones)", "Ports");
                menuItem.section = true;
                contextMenu.addMenuItem(menuItem);

                for (var i = 0; i < PulseObject.ports.length; i++) {
                    var port = PulseObject.ports[i];
                    var menuItem = newMenuItem();
                    menuItem.text = port.description;
                    if (port.availability == Port.Unavailable) {
                        if (port.name == "analog-output-speaker" || port.name == "analog-input-microphone-internal") {
                            menuItem.text += i18nc("Port is unavailable", " (unavailable)");
                        } else {
                            menuItem.text += i18nc("Port is unplugged", " (unplugged)");
                        }
                    }
                    menuItem.enabled = isMultiplePorts;
                    menuItem.checkable = true;
                    menuItem.checked = i === PulseObject.activePortIndex;
                    var setActivePort = function(portIndex) {
                        return function() {
                            PulseObject.activePortIndex = portIndex;
                        };
                    };
                    menuItem.clicked.connect(setActivePort(i));
                    contextMenu.addMenuItem(menuItem);
                }
            }
        }

        function show() {
            loadDynamicActions();
            openRelative();
        }
    }
}
