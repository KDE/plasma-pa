
import QtQuick 2.0
import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.volume 0.1
import org.kde.plasma.components 2.0 as Components
import org.kde.plasma.core 2.0 as PlasmaCore


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

    ListView {
        id: listView
        model: sinkModel
        onCountChanged: {
            console.debug("count changed" + count)
        }

        width: parent.width
        height: parent.height / 2
        header: Header { text: "Devices" }

        delegate: Components.ListItem {
            property int maximumWidth: parent.width
            property alias value: slider.value
            property alias maximumValue: slider.maximumValue
            Text {
                id: text
                anchors.top: parent.top
                text: Description
            }
            Components.Slider {
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

    ListView {
        id: inputView
        model: sinkInputModel

        onCountChanged: {
            console.debug("fuck count changed" + count)
        }

        height: parent.height
        anchors {
            top: listView.bottom
            left: parent.left
            right: parent.right
        }

        header: Header { text: "Applications" }

        delegate: SinkInputItem {
            maximumWidth: inputView.width
        }
    }

    Component.onCompleted: {
        sinkModel.setContext(pulseContext)
        sinkInputModel.setContext(pulseContext)
    }
}
