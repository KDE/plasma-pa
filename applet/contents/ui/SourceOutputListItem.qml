import QtQuick 2.0

import org.kde.plasma.components 2.0 as PlasmaComponents

import org.kde.plasma.private.volume 0.1

StreamListItemBase {
    // FIXME: all of this needs to be dependent on how many sources we have...
    expanderIconVisible: false
    enabled: true
    subComponent: PlasmaComponents.ComboBox {
        model: SourceModel {}
        textRole: 'Description'
        onModelChanged: updateIndex()
        onCountChanged: updateIndex()
        onActivated: {
            if (index === -1) {
                // Current index doesn't map to anything. Oh the agony.
                return;
            }
            PulseObject.sourceIndex = modelIndexToSourceIndex(index)
        }

        function updateIndex() {
            currentIndex = sourceIndexToModelIndex(SourceIndex);
        }

        function sourceIndexToModelIndex(sourceIndex) {
            textRole = 'Index';
            var searchString = '';
            if (sourceIndex !== 0) {
                // The stringy representation of 0 is '' oddly enough.
                searchString = '' + sourceIndex;
            }
            var modelIndex = find(searchString);
            textRole = 'Description';
            return modelIndex;
        }

        function modelIndexToSourceIndex(modelIndex) {
            textRole = 'Index';
            var sourceIndex = Number(textAt(modelIndex));
            textRole = 'Description';
            return sourceIndex;
        }

        Component.onCompleted: updateIndex();
    }
}
