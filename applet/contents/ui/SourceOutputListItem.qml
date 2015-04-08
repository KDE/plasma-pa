import QtQuick 2.0

import org.kde.plasma.components 2.0 as PlasmaComponents

import org.kde.plasma.private.volume 0.1

StreamListItemBase {
    // FIXME: all of this needs to be dependent on how many sinks we have...
    expanderIconVisible: false
    enabled: true
    subComponent: PlasmaComponents.ComboBox {
        property int sourceIndex: SourceIndex
        property int sourceModelIndex: model.paIndexToDataIndex(sourceIndex)
        property int sourceOutputIndex: Index
        model: SourceModel {
            Component.onCompleted: {
                setContext(pulseContext);
                sourceModelIndex = model.paIndexToDataIndex(sourceIndex)
            }
        }
        textRole: "Description"
        currentIndex: sourceModelIndex
        onCurrentIndexChanged: {
            if (sourceModelIndex === -1) {
                // Current index doesn't map to anything. Oh the agony.
                return;
            }

            if (currentIndex != sourceModelIndex) {
                // Context translates model index to PA index.
                pulseContext.setSourceOutputSinkByModelIndex(sourceOutputIndex, currentIndex);
            }
        }
    }

    function setVolume(volume) {
        pulseContext.setSinkInputVolume(Index, volume);
    }
}
