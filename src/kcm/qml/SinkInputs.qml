
import QtQuick 2.0
//import org.kde.plasma.volume 0.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import org.kde.kquickcontrolsaddons 2.0

Rectangle {
//    color: "red"
    implicitWidth: column.implicitWidth
    implicitHeight: column.implicitHeight

    ColumnLayout {
        id: column
        anchors.fill: parent

        ScrollView {
            frameVisible: true
            highlightOnFocus: true
            Layout.fillWidth: true
            Layout.fillHeight: true
            ListView {
                id: inputView

                model: dataModel

                anchors.fill: parent
                anchors.margins: 10

                delegate: Item {
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
                                Text {
                                    id: inputText
                                    Layout.fillWidth: true
                                    text: ClientName + ": " + Name
                                }

                                Button {
                                    iconName: 'audio-volume-muted'
                                }
                            }
                            RowLayout {
                                Text {
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
                                        pulseContext.setSinkInputVolume(Index, value)
                                    }

                                    Component.onCompleted: {
                                        console.debug("delegate")
                                        if (!HasVolume || !IsVolumeWritable)
                                            return
                                        inputSlider.value = Volume
                                    }
                                }

                                Text {
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

            }

        }

    }
}
