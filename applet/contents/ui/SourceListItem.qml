import QtQuick 2.0

import org.kde.plasma.volume 0.1

BaseDeviceItem {
    icon: 'audio-input-microphone'
    subModel: ReverseFilterModel {
        sortRole: SourceOutputModel.IndexRole
        filterRole: SourceOutputModel.SourceIndexRole;
        filterRegExp: new RegExp(Index);
        sourceModel: SourceOutputModel {
            Component.onCompleted: setContext(pulseContext);
        }

        Component.onCompleted: sort();
    }

    // FIXME: implement
//    function setVolume(volume) {
//    }
}
