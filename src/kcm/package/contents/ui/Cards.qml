import QtQuick 2.0
import QtQuick.Controls 1.0

import org.kde.plasma.private.volume 0.1

ThemedScrollView {
    frameVisible: false
    highlightOnFocus: true
    anchors.fill: parent

    ListView {
        id: listView

        anchors.fill: parent
        // FIXME: arbitrary value
        anchors.margins: 10
        model: CardModel {}
        spacing: anchors.margins
        delegate: CardListItem {}
    }
}
