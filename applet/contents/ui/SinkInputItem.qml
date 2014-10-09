import QtQuick 2.0

import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

import org.kde.kquickcontrolsaddons 2.0
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.core 2.0 as PlasmaCore

PlasmaComponents.ListItem {
    id: item

    anchors {
        left: parent.left;
        right: parent.right;
    }

    RowLayout {
        property int maximumWidth: parent.width
        width: maximumWidth
        Layout.maximumWidth: maximumWidth
        spacing: 8

        QIconItem {
            id: clientIcon
            Layout.alignment: Qt.AlignHCenter
            width: height
            height: column.height * 0.75
            icon: {
                if (ClientProperties['application.icon_name']) {
                    return ClientProperties['application.icon_name'].toLowerCase();
                } else if (ClientProperties['application.process.binary']) {
                    if (ClientProperties['application.process.binary'].toLowerCase() === 'chrome') {
                        return 'google-chrome';
                    }
                    return ClientProperties['application.process.binary'].toLowerCase();
                }
                return 'unknown';
            }
        }

        ColumnLayout {
            id: column

            RowLayout {
                PlasmaComponents.Label {
                    id: inputText
                    Layout.fillWidth: true
                    text: ClientName + ": " + Name
                    elide: Text.ElideRight
                }

                // The volume icon is packed into an item because we need to have it centered
                // with regards to the percent text, so we can potentially have a wider item
                // with a centered less wide icon.
                Item {
                    // Layout might want to make the text's height bigger than the content
                    // because of us, so we have to go after contentHeight to make sure we get the
                    // correct reference value, also we'd make an indirect binding loop otherwise.
                    width: Math.max(referenceText.width, inputText.contentHeight)
                    height: inputText.contentHeight

                    PlasmaCore.SvgItem {
                        id: muteIcon;

                        anchors.centerIn: parent
                        height: parent.height
                        width: height

                        elementId: {
                            var split_base = 65536/3.0;
                            var icon = null;
                            if (Volume / split_base <= 0) {
                                icon = "audio-volume-muted";
                            } else if (Volume / split_base <= 1) {
                                icon = "audio-volume-low";
                            } else if (Volume / split_base <= 2) {
                                icon = "audio-volume-medium";
                            } else {
                                icon = "audio-volume-high";
                            }
                            return icon;
                        }
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
                PlasmaComponents.Label {
                    id: percentText
                    Layout.alignment: Qt.AlignHCenter
                    Layout.minimumWidth: referenceText.width
                    horizontalAlignment: Qt.AlignRight
                    text: Math.floor(slider.value / slider.maximumValue * 100.0) + "%"
                }
            }
        }
    }

    PlasmaComponents.Label {
        id: referenceText
        visible: false
        text: "100%"
    }
}
