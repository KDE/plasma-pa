import QtQuick 2.0

import org.kde.plasma.volume 0.1

BaseItem {
    property QtObject subModel

    label: Description
    subCount: pseudoView.count
    subComponent: ListView {
        id: inputView

        width: parent ? parent.width : 0
        //        height: contentHeight

        model: subModel
        boundsBehavior: Flickable.StopAtBounds;
        delegate: SinkInputItem {}
    }

    ListView {
        id: pseudoView
        visible: false
        model: subModel
        delegate: Item {}
    }
}
