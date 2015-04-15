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
            text: Description
        }

        Button {
            iconName: 'audio-volume-muted'
            onClicked: PulseObject.muted = !PulseObject.muted
        }
    }

    RowLayout {
        Label {
            text: 'Port'
        }

        ComboBox {
            Layout.fillWidth: true
            model: Ports
            textRole: "description"
            currentIndex: -1
            onCurrentIndexChanged: {
                // Get Name
                textRole = "name";
                var name = currentText;
                textRole = "description";
                kcm.context.setSinkPort(Index, name);
            }
        }
    }

    RowLayout {
        Label {
            text: 'mono'
        }

        Slider {
            id: inputSlider

            property int volume: Volume
            property bool applyingVolume: false

            Layout.fillWidth: true
            anchors {
                top: inputText.bottom
            }
            minimumValue: 0
            maximumValue: 65536
            stepSize: maximumValue / 100
            focus: true
            visible: (HasVolume && IsVolumeWritable) ? true : false
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
                if (!HasVolume || !IsVolumeWritable)
                    return
                inputSlider.value = Volume
            }
        }

        Label {
            text: '100 '
        }
    }

    Rectangle {
        color: "grey"
        //                                width: parent.width
        Layout.fillWidth: true
        height: 1
    }
}
