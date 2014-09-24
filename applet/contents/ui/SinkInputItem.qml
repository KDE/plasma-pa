import QtQuick 2.0

import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

import org.kde.kquickcontrolsaddons 2.0
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.core 2.0 as PlasmaCore

PlasmaComponents.ListItem {
    property alias maximumWidth: column.maximumWidth

    ColumnLayout {
        id: column
        property int maximumWidth: parent.width
        width: maximumWidth
        Layout.maximumWidth: maximumWidth
        spacing: 8

        RowLayout {

            QIconItem {
                id: clientIcon
                Layout.alignment: Qt.AlignHCenter
                height: parent.height / 3 * 1.5
                width: height
                icon: ClientProperties['application.icon_name'] ? ClientProperties['application.icon_name'] : 'unknown'
            }

            ColumnLayout {
                RowLayout {
                    Text {
                        id: inputText
                        Layout.fillWidth: true
                        text: ClientName + ": " + Name
                        elide: Text.ElideRight
                    }

                    Item {
                        Layout.maximumWidth: Math.max(referenceText.width, Layout.maximumHeight)
                        Layout.maximumHeight: inputText.height

                        width: Layout.maximumWidth
                        height: Layout.maximumHeight

                        PlasmaCore.SvgItem {
                            id: muteIcon;

                            anchors.centerIn: parent
                            height: inputText.height
                            width: height

                            elementId: 'audio-volume-muted'
                            svg: PlasmaCore.FrameSvg { imagePath: "icons/audio" }

                            MouseArea {
                                anchors.fill: parent
                                hoverEnabled: true

                                onClicked: {
                                    // TODO: mute
                                }
                            }
                        }
                    }

                }

                RowLayout {
                    PlasmaComponents.Slider {
                        id: slider
                        Layout.fillWidth: true
                        minimumValue: 0
                        maximumValue: 65536
                        stepSize: maximumValue / 100
                        visible: (HasVolume && IsVolumeWritable) ? true : false
                        onValueChanged: {
                            pulseContext.setSinkInputVolume(Index, value)
                        }

                        Component.onCompleted: {
                            console.debug("+++ created " + Name)
                            if (!HasVolume || !IsVolumeWritable)
                                return
                            value = Volume
                        }
                    }
                    Text {
                        id: percentText
                        Layout.alignment: Qt.AlignHCenter
                        Layout.minimumWidth: referenceText.width
                        horizontalAlignment: Qt.AlignRight
                        text: Math.floor(slider.value / slider.maximumValue * 100.0) + "%"
                    }
                }
            }
        }

        Loader {
            id: expansionLoader;
            Layout.fillWidth: true
        }
    }

    Text {
        id: referenceText
        visible: false
        text: "100%"
    }

}
