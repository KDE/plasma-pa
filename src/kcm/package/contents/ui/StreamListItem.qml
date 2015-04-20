import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

import org.kde.kquickcontrolsaddons 2.0

import org.kde.plasma.private.volume 0.1

Item {
    id: item

    height: delegateColumn.height
    width: parent.width

    RowLayout {
        property int maximumWidth: parent.width - (4 * spacing)
        width: maximumWidth
        Layout.maximumWidth: maximumWidth
        spacing: 8

        ClientIcon {
            id: clientIcon

            height: parent.height / 3 * 1.5
            width: height
            anchors.verticalCenter: parent.verticalCenter
            client: PulseObject.client
        }

        ColumnLayout {
            id: delegateColumn
            anchors.left: clientIcon.right
            RowLayout {
                Label {
                    id: inputText
                    Layout.fillWidth: true
                    text: PulseObject.client.name + ": " + PulseObject.name
                }

                Button {
                    iconName: 'audio-volume-muted'
                    checkable: true
                    checked: PulseObject.muted
                    onClicked: PulseObject.muted = !PulseObject.muted
                }
            }

            VolumeSlider {}

            Rectangle {
                color: "grey"
                //                                width: parent.width
                Layout.fillWidth: true
                height: 1
            }
        }

    }

}
