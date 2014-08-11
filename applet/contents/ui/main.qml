
import QtQuick 2.0
import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.volume 0.1
import org.kde.plasma.components 2.0 as Components


Item {
    Plasmoid.icon: "audio-volume-medium";

    Context {
        id: pulseContext
    }

    SinkModel {
        id: clientModel
    }

    ListView {
        id: listView
        model: clientModel
        onCountChanged: {
            console.debug("count changed" + count)
        }

        width: parent.width
        height: parent.height / 2
        delegate: Item {
            Text {
                id: text
                anchors.top: parent.top
                text: description
            }
            Components.Slider {
                id: slider
                anchors {
                    top: text.bottom
                }
                minimumValue: 0
                maximumValue: 65536
                stepSize: maximumValue / 100
                focus: true
                onValueChanged: {
                    pulseContext.setSinkVolume(index, value)
                }
            }
            Component.onCompleted: slider.value = volume
        }
    }

    SinkInputModel {
        id: sinkInputModel
        onRowsInserted: {
            console.debug("inserto")
        }
    }

    ListView {
        id: inputView
        model: sinkInputModel

        onCountChanged: {
            console.debug("fuck count changed" + count)
        }
        
        width: parent.width
        anchors.top: listView.bottom
        delegate: Item {
            Text {
                id: inputText
                anchors.top: parent.top
                text: Name
            }
            Components.Slider {
                id: inputSlider
                anchors {
                    top: inputText.bottom
                }
                minimumValue: 0
                maximumValue: 65536
                stepSize: maximumValue / 100
                focus: true
                visible: (HasVolume && IsVolumeWritable) ? true : false
                onValueChanged: {
                    pulseContext.setSinkInputVolume(Index, value)
                }
            }
            Component.onCompleted: {
                if (!HasVolume || !IsVolumeWritable)
                    return
                inputSlider.value = Volume
            }
        }
    }

    Component.onCompleted: {
        clientModel.setContext(pulseContext)
        sinkInputModel.setContext(pulseContext)
    }
}
