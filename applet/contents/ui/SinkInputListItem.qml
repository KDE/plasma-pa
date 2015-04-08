import QtQuick 2.0

import org.kde.plasma.components 2.0 as PlasmaComponents

import org.kde.plasma.private.volume 0.1

StreamListItemBase {
    // FIXME: all of this needs to be dependent on how many sinks we have...
    expanderIconVisible: false
    enabled: true
    subComponent: PlasmaComponents.ComboBox {
        property int sinkIndex: SinkIndex
        property int sinkModelIndex: model.paIndexToDataIndex(sinkIndex)
        property int sinkInputIndex: Index
        model: SinkModel {
            context: pulseContext
            Component.onCompleted: {
                sinkModelIndex = model.paIndexToDataIndex(sinkIndex)
            }
        }
        textRole: "Description"
        currentIndex: sinkModelIndex
        onCurrentIndexChanged: {
            if (sinkModelIndex === -1) {
                // Current index doesn't map to anything. Oh the agony.
                return;
            }

            if (currentIndex != sinkIndex) {
                pulseContext.setSinkInputSinkByModelIndex(sinkInputIndex, currentIndex);
            }
        }
    }

    function setVolume(volume) {
        pulseContext.setSinkInputVolume(Index, volume);
    }
}
