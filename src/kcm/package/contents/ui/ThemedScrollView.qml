import QtQuick 2.0
import QtQuick.Controls 1.0

ScrollView {
    frameVisible: false

    Rectangle {
        color: systemPalette.window
        anchors.fill: parent

        SystemPalette {
            id: systemPalette
        }
    }
}
