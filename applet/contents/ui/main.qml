
import QtQuick 2.0
import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.volume 0.1
import org.kde.plasma.components 2.0 as Components


Item {
//    width: 680
//    height: 420

    Plasmoid.icon: "audio-volume-medium";

    Context {
        id: pulseContext
    }

    SinkModel {
        id: clientModel
    }

    ListView {
        model: clientModel
        onCountChanged: {
            console.debug("count changed" + count)
        }

        width: parent.width
        height: parent.height
        delegate: Item {
            Text {
                id: text
                anchors.top: parent.top
                text: 'name: ' + name + "  volume: " + volume
            }
            Components.Slider {
                id: slider
                anchors {
                    top: text.bottom
                }
                minimumValue: 0
                maximumValue: 65536
                stepSize: maximumValue / 10
                focus: true
                onValueChanged: pulseContext.setVolume(index, value)
            }
            Component.onCompleted: slider.value = volume
        }
    }

    Component.onCompleted: {
        clientModel.setContext(pulseContext)
    }

}
