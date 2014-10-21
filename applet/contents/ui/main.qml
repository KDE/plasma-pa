import QtQuick 2.0

import QtQuick.Layouts 1.0

import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras
import org.kde.plasma.plasmoid 2.0

import org.kde.plasma.volume 0.1

Item {
    Plasmoid.icon: "audio-volume-medium";

    Plasmoid.switchWidth: units.gridUnit * 12
    Plasmoid.switchHeight: units.gridUnit * 12

    Plasmoid.toolTipMainText: i18n("Audio Volume")
    Plasmoid.toolTipSubText: sinkModel.volumeText

    Plasmoid.compactRepresentation: PlasmaCore.IconItem {
        source: plasmoid.icon ? plasmoid.icon : "plasma"
        active: mouseArea.containsMouse

        MouseArea {
            id: mouseArea

            property bool wasExpanded: false

            anchors.fill: parent
            hoverEnabled: true
            onPressed: wasExpanded = plasmoid.expanded
            onClicked: plasmoid.expanded = !wasExpanded
            onWheel: {
                if (sinkView.count < 0)
                    return;
                for (var i = 0; i < sinkView.count; ++i) {
                    sinkView.currentIndex = i;
                    if (wheel.angleDelta.y > 0) {
                        sinkView.currentItem.increaseVolume()
                    } else {
                        sinkView.currentItem.decreaseVolume()
                    }
                }
            }
        }
    }

    Context {
        id: pulseContext
    }

    PlasmaExtras.ScrollArea {
        id: scrollView;

        anchors {
            bottom: parent.bottom;
            left: parent.left;
            right: parent.right;
            top: parent.top;
            rightMargin: 16
        }

        ColumnLayout {
            property int maximumWidth: scrollView.viewport.width
            width: maximumWidth
            Layout.maximumWidth: maximumWidth

            ListView {
                id: sinkView

                Layout.fillWidth: true
                Layout.minimumHeight: contentHeight

                model: SinkModel {
                    id: sinkModel
                    Component.onCompleted: {
                        setContext(pulseContext);
                    }
                }
                boundsBehavior: Flickable.StopAtBounds;
                delegate: SinkItem {}
            }

            ListView {
                id: sourceView

                Layout.fillWidth: true
                Layout.minimumHeight: contentHeight

                model: SourceModel {
                    id: sourceModel
                    Component.onCompleted: {
                        setContext(pulseContext);
                    }
                }
                boundsBehavior: Flickable.StopAtBounds;
                delegate: SourceListItem {}
            }
        }
    }
}
