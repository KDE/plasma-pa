import QtQuick 2.0

ThemedScrollView {
    property alias model: view.model
    property alias delegate: view.delegate

    frameVisible: true
    highlightOnFocus: true
    anchors.fill: parent

    ListView {
        id: view
        anchors.fill: parent
        anchors.margins: 10
    }
}
