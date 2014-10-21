import QtQuick 2.0

import org.kde.plasma.volume 0.1

BaseItem {
    icon: 'audio-card'
    label: Description
    subCount: pseudoView.count
    subComponent: ListView {
        id: inputView

        width: parent ? parent.width : 0
        //        height: contentHeight

        model:sinkInputModel
        boundsBehavior: Flickable.StopAtBounds;
        delegate: SinkInputItem {}
    }

    ListView {
        id: pseudoView
        visible: false
        model: sinkInputModel
        delegate: Item {}
    }

    ReverseSinkInputModel {
        id: sinkInputModel
        filterRole: SinkInputModel.SinkIndexRole
        filterRegExp: new RegExp(Index);
        Component.onCompleted: {
            sinkInputModel.setContext(pulseContext);
        }
    }

    function setVolume(volume) {
        // TODO: possibly set the volume icon here, otherwise it will update on delay
        pulseContext.setSinkVolume(Index, volume);
    }
}
