import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

import org.kde.kquickcontrolsaddons 2.0

Item {
    id: item

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
            icon: ClientProperties['application.icon_name'] ? ClientProperties['application.icon_name'] : 'unknown'
        }

        ColumnLayout {
            id: delegateColumn
            anchors.left: clientIcon.right
            RowLayout {
                Label {
                    id: inputText
                    Layout.fillWidth: true
                    text: ClientName + ": " + Name
                }

                Button {
                    iconName: 'audio-volume-muted'
                }
            }
            RowLayout {
                Label {
                    text: 'mono'
                }

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
                    visible: (HasVolume && IsVolumeWritable) ? true : false
                    onValueChanged: {
                        PulseObject.volume = value
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

    }

}
