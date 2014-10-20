import QtQuick 2.0

import org.kde.plasma.volume 0.1

BaseItem {
    icon: 'audio-card'
    label: Description
    subComponent: ListView {
            id: inputView

            width: parent.width
            height: contentHeight

            model: ReverseSinkInputModel {
                id: m
                filterRole: SinkInputModel.SinkIndexRole
                filterRegExp: new RegExp(Index);
                Component.onCompleted: {
                    m.setContext(pulseContext);
                }
            }
            boundsBehavior: Flickable.StopAtBounds;
            delegate: SinkInputItem {}
        }

    function setVolume(volume) {
        // TODO: possibly set the volume icon here, otherwise it will update on delay
        pulseContext.setSinkVolume(Index, volume);
    }
}
