
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
                            height: parent.height / 3
                            width: height
                            anchors.verticalCenter: parent.verticalCenter
                            icon: 'audio-card'
                        }

                        ColumnLayout {
                            id: delegateColumn
                            anchors.left: clientIcon.right
                            RowLayout {
                                Text {
                                    id: inputText
                                    Layout.fillWidth: true
                                    text: Description
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
                                        pulseContext.setSinkVolume(Index, value)
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

                                Text {
                                    text: '100 '
                                }
                            }

                            RowLayout {
                                Text {
                                    text: 'Port'
                                }

                                ComboBox {
                                    Layout.fillWidth: true
                                    currentIndex: ActivePortIndex
                                    model: Ports
                                    width: 200
                                    onCurrentIndexChanged: {

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

            }

        }

    }
}
