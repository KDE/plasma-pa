import QtQuick 2.0
import QtQuick.Layouts 1.0

import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras
import org.kde.plasma.plasmoid 2.0

import org.kde.plasma.private.volume 0.1

import "../code/icon.js" as Icon

Item {
    Plasmoid.icon: sinkModel.sinks.length > 0 ? Icon.name(sinkModel.sinks[0].volume, sinkModel.sinks[0].muted) : Icon.name(0, true)

    Plasmoid.switchWidth: units.gridUnit * 12
    Plasmoid.switchHeight: units.gridUnit * 12

    Plasmoid.toolTipMainText: i18n("Audio Volume")
    Plasmoid.toolTipSubText: sinkModel.volumeText

    Plasmoid.compactRepresentation: PlasmaCore.IconItem {
        source: plasmoid.icon
        active: mouseArea.containsMouse

        MouseArea {
            id: mouseArea

            property bool wasExpanded: false

            anchors.fill: parent
            hoverEnabled: true
            acceptedButtons: Qt.LeftButton | Qt.MiddleButton
            onPressed: {
              if (mouse.button == Qt.LeftButton) {
                wasExpanded = plasmoid.expanded
              } else if (mouse.button == Qt.MiddleButton) {
                for (var i in sinkModel.sinks) {
                  var sink = sinkModel.sinks[i]
                  sink.muted = !sink.muted
                }
              }
            }
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
                Layout.maximumHeight: contentHeight

                model: SinkModel {
                    id: sinkModel
                }
                boundsBehavior: Flickable.StopAtBounds;
                delegate: SinkListItem {}
            }

            ListView {
                id: sourceView

                Layout.fillWidth: true
                Layout.minimumHeight: contentHeight
                Layout.maximumHeight: contentHeight

                model: SourceModel {
                    id: sourceModel
                }
                boundsBehavior: Flickable.StopAtBounds;
                delegate: SourceListItem {}
            }
        }
    }
}
