
import QtQuick 2.0

import QtQuick.Layouts 1.0

import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras
import org.kde.plasma.plasmoid 2.0

import org.kde.plasma.volume 0.1

Item {
    Plasmoid.icon: "audio-volume-medium";

    Plasmoid.compactRepresentation: PlasmaCore.IconItem {
        id: icon

        source: plasmoid.icon ? plasmoid.icon : "plasma"
        active: mouseArea.containsMouse

        MouseArea {
            id: mouseArea

            property bool wasExpanded: false

            anchors.fill: parent
            hoverEnabled: true
            onPressed: wasExpanded = plasmoid.expanded
            onClicked: plasmoid.expanded = !wasExpanded
            onWheel: {
                console.debug("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
                console.debug(wheel.angleDelta);
                if (listView.count < 0)
                    return;
                listView.currentIndex = 0;
                var step = listView.currentItem.maximumValue / 10;
                if (wheel.angleDelta.y > 0) {
                    listView.currentItem.value += step;
                } else {
                    listView.currentItem.value -= step;
                }
            }
        }
    }

    Context {
        id: pulseContext
    }

    SinkModel {
        id: sinkModel
    }

    SinkInputModel {
        id: sinkInputModel
        onRowsInserted: {
            console.debug("+++ inserto")
        }
        onDataChanged: {
            console.debug("+++ changeroo")
        }
        onModelReset: {
            console.debug("+++ model reset")
        }
    }

    PlasmaExtras.ScrollArea {
        id: scrollView;

        anchors {
            bottom: parent.bottom;
            left: parent.left;
            right: parent.right;
            top: parent.top;
        }

        ColumnLayout {
            property int maximumWidth: scrollView.viewport.width
            width: maximumWidth
            Layout.maximumWidth: maximumWidth

            ListView {
                id: listView

                Layout.fillWidth: true
                Layout.minimumHeight: contentHeight

                model: sinkModel
                boundsBehavior: Flickable.StopAtBounds;
                header: Header { text: "Devices" }
                delegate: PlasmaComponents.ListItem {
                    property int maximumWidth: parent.width
                    property alias value: slider.value
                    property alias maximumValue: slider.maximumValue
                    Text {
                        id: text
                        anchors.top: parent.top
                        text: Description
                    }
                    PlasmaComponents.Slider {
                        id: slider
                        anchors {
                            top: text.bottom
                        }
                        width:parent.width
                        minimumValue: 0
                        maximumValue: 65536
                        stepSize: maximumValue / 100
                        focus: true
                        onValueChanged: {
                            pulseContext.setSinkVolume(Index, value)
                            if (index == 0) {
                                var split_base = maximumValue/3.0;
                                if (value / split_base <= 0) {
                                    plasmoid.icon = "audio-volume-muted";
                                } else if (value / split_base <= 1) {
                                    plasmoid.icon = "audio-volume-low";
                                } else if (value / split_base <= 2) {
                                    plasmoid.icon = "audio-volume-medium";
                                } else {
                                    plasmoid.icon = "audio-volume-high";
                                }
                            }
                        }
                    }
                    Component.onCompleted: slider.value = Volume
                }
            }

            ListView {
                id: inputView

                Layout.fillWidth: true
                Layout.minimumHeight: contentHeight

                model: sinkInputModel
                boundsBehavior: Flickable.StopAtBounds;
                header: Header { text: "Applications" }
                delegate: SinkInputItem {}
            }
        }
    }

    Component.onCompleted: {
        sinkModel.setContext(pulseContext)
        sinkInputModel.setContext(pulseContext)
    }
}
