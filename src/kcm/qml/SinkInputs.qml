import QtQuick 2.0

ThemedScrollView {
    frameVisible: true
    highlightOnFocus: true
    anchors.fill: parent

    ListView {
        id: inputView

        model: dataModel
        anchors.fill: parent
        anchors.margins: 10
        delegate: SinkInputListItem {}
    }
}
