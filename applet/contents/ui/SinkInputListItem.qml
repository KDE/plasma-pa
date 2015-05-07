import QtQuick 2.0

import org.kde.plasma.components 2.0 as PlasmaComponents

import org.kde.plasma.private.volume 0.1

StreamListItemBase {
    // FIXME: all of this needs to be dependent on how many sinks we have...
    expanderIconVisible: false
    enabled: true
    subComponent: PlasmaComponents.ComboBox {
        model: SinkModel {}
        textRole: 'Description'
        onModelChanged: updateIndex()
        onCountChanged: updateIndex()
        onActivated: {
            if (index === -1) {
                // Current index doesn't map to anything. Oh the agony.
                return;
            }
            PulseObject.sinkIndex = modelIndexToSinkIndex(index)
        }

        function updateIndex() {
            currentIndex = sinkIndexToModelIndex(SinkIndex);
        }

        function sinkIndexToModelIndex(sinkIndex) {
            textRole = 'Index';
            var searchString = '';
            if (sinkIndex !== 0) {
                // The stringy representation of 0 is '' oddly enough.
                searchString = '' + sinkIndex;
            }
            var modelIndex = find(searchString);
            textRole = 'Description';
            return modelIndex;
        }

        function modelIndexToSinkIndex(modelIndex) {
            textRole = 'Index';
            var sinkIndex = Number(textAt(modelIndex));
            textRole = 'Description';
            return sinkIndex;
        }

        Component.onCompleted: updateIndex();
    }
}
