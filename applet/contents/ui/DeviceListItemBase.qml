import QtQuick 2.0

import org.kde.plasma.private.volume 0.1

ListItemBase {
    property QtObject subModel
    property Component subDelegate

    label: Description
    expanderIconVisible: pseudoView.count > 0
    subComponent: ListView {
        id: inputView

        width: parent ? parent.width : 0
                height: contentHeight

        model: subModel
        boundsBehavior: Flickable.StopAtBounds;
        delegate: subDelegate
    }

    ListView {
        id: pseudoView
        visible: false
        model: subModel
        delegate: Item {}
    }
}
