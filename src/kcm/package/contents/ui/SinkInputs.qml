import QtQuick 2.0

import org.kde.plasma.private.volume 0.1

ThemedScrollView {
    frameVisible: true
    highlightOnFocus: true
    anchors.fill: parent

    ListView {
        id: inputView

        model: SinkInputModel {}
        anchors.fill: parent
        anchors.margins: 10
        delegate: SinkInputListItem {}
    }
}
