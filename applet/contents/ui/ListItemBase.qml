import QtQuick 2.0

import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

import org.kde.kquickcontrolsaddons 2.0
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.extras 2.0 as PlasmaExtras

PlasmaComponents.ListItem {
    id: item

    property bool expanded: false
    property string icon
    property Component subComponent

    property alias label: textLabel.text
    property alias expanderIconVisible: expanderIcon.visible

    enabled: subComponent

    function setVolume(volume) {
        if (volume > 0 && Muted) {
            toggleMute();
        }
        Volume = volume
    }

    function bound(value, min, max) {
        return Math.max(min, Math.min(value, max));
    }

    // FIXME: increase/decrease are also present on app streams as they derive
    //        from this, they are not used there though.
    //        seems naughty.
    function increaseVolume() {
        var step = slider.maximumValue / 15;
        var volume = bound(PulseObject.volume + step, 0, slider.maximumValue);
        setVolume(volume);
        osd.show(100 * volume / slider.maximumValue);
    }

    function decreaseVolume() {
        var step = slider.maximumValue / 15;
        var volume = bound(PulseObject.volume - step, 0, slider.maximumValue);
        setVolume(volume);
        osd.show(100 * volume / slider.maximumValue);
    }

    function toggleMute() {
        Muted = !Muted;
        osd.show(0);
    }

    anchors {
        left: parent.left;
        right: parent.right;
    }

    onIconChanged: {
        clientIcon.visible = icon ? true : false;
        clientIcon.icon = icon
    }

    ColumnLayout {
        property int maximumWidth: parent.width
        width: maximumWidth
        Layout.maximumWidth: maximumWidth

        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            QIconItem {
                id: clientIcon
                visible: false
                Layout.alignment: Qt.AlignHCenter
                width: height
                height: column.height * 0.75
            }

            ColumnLayout {
                id: column

                Item {
                    Layout.fillWidth: true
                    height: textLabel.height

                    PlasmaExtras.Heading {
                        id :textLabel
                        anchors.top: parent.top
                        anchors.left: parent.left
                        anchors.right: expanderIcon.visible ? expanderIcon.left : parent.right
                        //                    anchors.verticalCenter: iconContainer.verticalCenter
                        level: 5
                        opacity: 0.6
                        wrapMode: Text.NoWrap
                        elide: Text.ElideRight
                    }

                    PlasmaCore.SvgItem {
                        id: expanderIcon
                        visible: subComponent
                        anchors.top: parent.top;
                        anchors.right: parent.right;
                        anchors.bottom: parent.bottom;
                        width: height
                        svg: PlasmaCore.Svg {
                            imagePath: "widgets/arrows"
                        }
                        elementId: expanded ? "up-arrow" : "down-arrow"
                    }
                }

                RowLayout {
                    VolumeIcon {
                        Layout.maximumHeight: slider.height * 0.75
                        Layout.maximumWidth: slider.height* 0.75
                        volume: PulseObject.volume
                        muted: PulseObject.muted

                        MouseArea {
                            anchors.fill: parent
                            onPressed: PulseObject.muted = !PulseObject.muted
                        }
                    }

                    PlasmaComponents.Slider {
                        id: slider

                        // Helper property to allow async slider updates.
                        // While we are sliding we must not react to value updates
                        // as otherwise we can easily end up in a loop where value
                        // changes trigger volume changes trigger value changes.
                        property int volume: PulseObject.volume

                        Layout.fillWidth: true
                        minimumValue: 0
                        // FIXME: I do wonder if exposing max through the model would be useful at all
                        maximumValue: 65536
                        stepSize: maximumValue / 100
                        visible: PulseObject.hasVolume
                        enabled: {
                            if (typeof PulseObject.volumeWritable === 'undefined') {
                                return !PulseObject.muted
                            }
                            return PulseObject.volumeWritable && !PulseObject.muted
                        }

                        onVolumeChanged: {
                            if (!pressed) {
                                value = PulseObject.volume;
                            }
                        }

                        onValueChanged: {
                            if (pressed) {
                                setVolume(value);
                            }
                        }

                        onPressedChanged: {
                            if (!pressed) {
                                // Make sure to sync the volume once the button was
                                // released.
                                // Otherwise it might be that the slider is at v10
                                // whereas PA rejected the volume change and is
                                // still at v15 (e.g.).
                                value = PulseObject.volume;
                            }
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

        Loader {
            id: subLoader

            anchors.right: parent.right
            anchors.left: parent.left
            // FIXME: need something more dynamic
            anchors.leftMargin: 22

            Layout.minimumHeight: subLoader.item ? subLoader.item.height : 0
            Layout.maximumHeight: Layout.minimumHeight
        }
    }

    PlasmaComponents.Label {
        id: referenceText
        visible: false
        text: "100%"
    }

    states: [
        State {
            name: "collapsed";
            when: !expanded;
            StateChangeScript {
                script: {
                    if (subLoader.status == Loader.Ready) {
                        subLoader.sourceComponent = undefined;
                    }
                }
            }
        },
        State {
            name: "expanded";
            when: expanded;
            StateChangeScript {
                script: subLoader.sourceComponent = subComponent;
            }
        }
    ]

    onClicked: {
        if (!subComponent) {
            return;
        }
        expanded = !expanded;
    }
}
