import QtQuick 2.0
import QtQuick.Controls 1.0

ThemedScrollView {
    frameVisible: false
    highlightOnFocus: true
    anchors.fill: parent

    ListView {
        id: listView

        anchors.fill: parent
        // FIXME: arbitrary value
        anchors.margins: 10
        model: dataModel
        spacing: anchors.margins
        delegate: CardListItem {}
    }
}
