import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

import org.kde.kquickcontrolsaddons 2.0

ColumnLayout {
    id: delegateColumn
    width: parent.width

    RowLayout {
        QIconItem {
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
            checkable: true
            checked: PulseObject.muted
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

        VolumeSlider {}
    }

    Rectangle {
        color: "grey"
        //                                width: parent.width
        Layout.fillWidth: true
        height: 1
    }
}
