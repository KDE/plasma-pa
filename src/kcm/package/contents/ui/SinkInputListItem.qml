import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

import org.kde.kquickcontrolsaddons 2.0

import org.kde.plasma.private.volume 0.1

Item {
    id: item

    property variant sinkInput: PulseObject

    height: delegateColumn.height
    width: parent.width

    RowLayout {
        property int maximumWidth: parent.width - (4 * spacing)
        width: maximumWidth
        Layout.maximumWidth: maximumWidth
        spacing: 8

        QIconItem {
            id: clientIcon
            height: parent.height / 3 * 1.5
            width: height
            anchors.verticalCenter: parent.verticalCenter
            icon: sinkInput.client.properties['application.icon_name'] ? sinkInput.client.properties['application.icon_name'] : 'unknown'
        }

        ColumnLayout {
            id: delegateColumn
            anchors.left: clientIcon.right
            RowLayout {
                Label {
                    id: inputText
                    Layout.fillWidth: true
                    text: sinkInput.client.name + ": " + sinkInput.name
                }

                Button {
                    iconName: 'audio-volume-muted'
                    onClicked: sinkInput.muted = !sinkInput.muted
                }
            }
            RowLayout {
                Slider {
                    id: inputSlider
                    Layout.fillWidth: true
                    anchors {
                        top: inputText.bottom
                    }
                    minimumValue: 0
                    maximumValue: 65536
                    stepSize: maximumValue / 100
                    focus: true
                    visible: (sinkInput.hasVolume && sinkInput.isVolumeWritable) ? true : false
                    onValueChanged: {
                        sinkInput.volume = value
                    }

                    Component.onCompleted: {
                        if (!sinkInput.hasVolume || !sinkInput.isVolumeWritable)
                            return
                        inputSlider.value = sinkInput.volume
                    }
                }
            }

            Rectangle {
                color: "grey"
                //                                width: parent.width
                Layout.fillWidth: true
                height: 1
            }
        }

    }

}
