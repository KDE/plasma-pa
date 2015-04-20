import QtQuick 2.0
import QtQuick.Controls 1.0

Button {
    property bool muted: true

    iconName: 'audio-volume-muted'
    checkable: true
    checked: muted
    onClicked: muted = !muted
}
