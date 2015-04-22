import QtQuick 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.0

RowLayout {
    // VolumeIcon {
    //     Layout.maximumHeight: slider.height * 0.75
    //     Layout.maximumWidth: slider.height* 0.75
    //     volume: PulseObject.volume
    //     muted: PulseObject.muted
    //
    //     MouseArea {
    //         anchors.fill: parent
    //         onPressed: PulseObject.muted = !PulseObject.muted
    //     }
    // }

    Slider {
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
        enabled: PulseObject.volumeWritable && !PulseObject.muted

        onVolumeChanged: {
            if (!pressed) {
                value = PulseObject.volume;
            }
        }

        onValueChanged: {
            if (pressed) {
                PulseObject.volume = value;
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

    Label {
        id: percentText
        Layout.alignment: Qt.AlignHCenter
        Layout.minimumWidth: referenceText.width
        horizontalAlignment: Qt.AlignRight
        text: Math.floor(slider.value / slider.maximumValue * 100.0) + "%"
    }

    Label {
        id: referenceText
        visible: false
        text: "100%"
    }
}
