import QtQuick 2.0

ThemedScrollView {
    frameVisible: true
    highlightOnFocus: true
    anchors.fill: parent

    ListView {
        id: inputView

        model: kcm.sinkModel
        anchors.fill: parent
        anchors.margins: 10
        delegate: SinkListItem {}
    }
}
