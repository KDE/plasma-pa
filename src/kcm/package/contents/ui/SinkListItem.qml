import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

import org.kde.kquickcontrolsaddons 2.0

ColumnLayout {
    id: delegateColumn
    width: parent.width

    RowLayout {
        QIconItem {
            id: clientIcon
            width: height
            height: inputText.contentHeight
            anchors.verticalCenter: parent.verticalCenter
            icon: 'audio-card'
        }

        Label {
            id: inputText
            Layout.fillWidth: true
            text: PulseObject.description
        }

        Button {
            iconName: 'audio-volume-muted'
            onClicked: PulseObject.muted = !PulseObject.muted
        }
    }

    ColumnLayout {
        width: parent.width
        enabled: !PulseObject.muted

        RowLayout {
            Label {
                text: 'Port'
            }

            ComboBox {
                Layout.fillWidth: true
                model: PulseObject.ports
                textRole: "description"
                currentIndex: PulseObject.activePortIndex
                onActivated: PulseObject.activePortIndex = index
                Connections {
                    target: PulseObject
                    // FIXME: comobox for some reason doesn't update after init
                    onActivePortIndexChanged: parent.currentIndex = PulseObject.activePortIndex
                }
            }
        }

        RowLayout {
            Slider {
                id: inputSlider

                property int volume: PulseObject.volume
                property bool applyingVolume: false

                Layout.fillWidth: true
                anchors {
                    top: inputText.bottom
                }
                minimumValue: 0
                maximumValue: 65536
                stepSize: maximumValue / 100
                focus: true
                onValueChanged: {
                    if (applyingVolume) {
                        applyingVolume = false
                        return
                    }

                    console.debug('changed')
                    PulseObject.volume = value
                }
                onVolumeChanged: {
                    applyingVolume = true
                    value = volume
                }

                Component.onCompleted: {
                    console.debug("delegate")
                    if (!PulseObject.hasVolume || !PulseObject.isVolumeWritable)
                        return
                    inputSlider.value = PulseObject.volume
                }
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
