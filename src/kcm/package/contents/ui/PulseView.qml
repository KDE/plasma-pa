import QtQuick 2.0
import QtQuick.Controls 1.0

ScrollView {
    property alias model: view.model
    property alias delegate: view.delegate

    frameVisible: false
    highlightOnFocus: true
    anchors.fill: parent

    ListView {
        id: view
        anchors.fill: parent
        anchors.margins: 10
        spacing: units.largeSpacing
    }
}
