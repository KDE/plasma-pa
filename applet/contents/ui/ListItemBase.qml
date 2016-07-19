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

PlasmaComponents.ListItem {
    id: item

    property alias label: textLabel.text
    property alias draggable: dragArea.enabled
    property alias icon: clientIcon.source
    property string type

    anchors {
        left: parent.left;
        right: parent.right;
    }

    checked: dropArea.containsDrag
    opacity: (draggedStream && draggedStream.deviceIndex == Index) ? 0.3 : 1.0

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

    ColumnLayout {
        property int maximumWidth: parent.width
        width: maximumWidth
        Layout.maximumWidth: maximumWidth

        RowLayout {
            Layout.fillWidth: true
            spacing: units.smallSpacing

            PlasmaCore.IconItem {
                id: clientIcon
                visible: valid
                Layout.alignment: Qt.AlignHCenter
                Layout.preferredHeight: column.height * 0.75
                Layout.preferredWidth: Layout.preferredHeight

                DragAndDrop.DragArea {
                    id: dragArea
                    anchors.fill: parent
                    delegate: parent

                    mimeData {
                        source: item
                    }

                    onDragStarted: {
                        draggedStream = PulseObject;
                        main.beginMoveStream(type == "sink-input" ? "sink" : "source");
                    }

                    onDrop: {
                        draggedStream = null;
                        main.endMoveStream();
                    }

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: dragArea.enabled ? (pressed ? Qt.ClosedHandCursor : Qt.OpenHandCursor) : undefined
                    }
                }
            }

            ColumnLayout {
                id: column

                PlasmaExtras.Heading {
                    id :textLabel
                    Layout.fillWidth: true
                    level: 5
                    opacity: 0.6
                    wrapMode: Text.NoWrap
                    elide: Text.ElideRight
                }

                RowLayout {
                    VolumeIcon {
                        Layout.maximumHeight: slider.height * 0.75
                        Layout.maximumWidth: slider.height* 0.75
                        volume: Volume
                        muted: Muted

                        MouseArea {
                            anchors.fill: parent
                            onPressed: Muted = !Muted
                        }
                    }

                    PlasmaComponents.Slider {
                        id: slider

                        // Helper properties to allow async slider updates.
                        // While we are sliding we must not react to value updates
                        // as otherwise we can easily end up in a loop where value
                        // changes trigger volume changes trigger value changes.
                        property int volume: Volume
                        property bool ignoreValueChange: false

                        Layout.fillWidth: true
                        minimumValue: PulseAudio.MinimalVolume
                        maximumValue: maxVolumeValue
                        stepSize: maximumValue / maxVolumePercent
                        visible: HasVolume
                        enabled: VolumeWritable
                        opacity: Muted ? 0.5 : 1

                        onVolumeChanged: {
                            ignoreValueChange = true;
                            value = Volume;
                            ignoreValueChange = false;
                        }

                        onValueChanged: {
                            if (!ignoreValueChange) {
                                Volume = value;
                                Muted = false;

                                if (type == "sink") {
                                    playFeedback(CardIndex);
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
    }
}
