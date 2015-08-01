import QtQuick 2.0
import QtQuick.Layouts 1.0

import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras
import org.kde.plasma.plasmoid 2.0

import org.kde.plasma.private.volume 0.1

import "../code/icon.js" as Icon

Item {
    id: main

    property string displayName: i18n("Audio Volume")

    Plasmoid.icon: sinkModel.sinks.length > 0 ? Icon.name(sinkModel.sinks[0].volume, sinkModel.sinks[0].muted) : Icon.name(0, true)
    Plasmoid.switchWidth: units.gridUnit * 12
    Plasmoid.switchHeight: units.gridUnit * 12
    Plasmoid.toolTipMainText: displayName
    // FIXME:    Plasmoid.toolTipSubText: sinkModel.volumeText

    function runOnAllSinks(func) {
        if (sinkView.count < 0)
            return;
        for (var i = 0; i < sinkView.count; ++i) {
            sinkView.currentIndex = i;
            sinkView.currentItem[func]();
        }
    }

    function increaseVolume() {
        runOnAllSinks("increaseVolume");
    }

    function decreaseVolume() {
        runOnAllSinks("decreaseVolume");
    }

    function muteVolume() {
        runOnAllSinks("toggleMute");
    }

    Plasmoid.compactRepresentation: PlasmaCore.IconItem {
        source: plasmoid.icon
        active: mouseArea.containsMouse
        colorGroup: PlasmaCore.ColorScope.colorGroup

        MouseArea {
            id: mouseArea

            property bool wasExpanded: false

            anchors.fill: parent
            hoverEnabled: true
            acceptedButtons: Qt.LeftButton | Qt.MiddleButton
            onPressed: {
                if (mouse.button == Qt.LeftButton) {
                    wasExpanded = plasmoid.expanded;
                } else if (mouse.button == Qt.MiddleButton) {
                    muteVolume();
                }
            }
            onClicked: {
                if (mouse.button == Qt.LeftButton) {
                    plasmoid.expanded = !wasExpanded;
                }
            }
            onWheel: {
                if (wheel.angleDelta.y > 0) {
                    increaseVolume();
                } else {
                    decreaseVolume();
                }
            }
        }
    }

    GlobalActionCollection {
        // KGlobalAccel cannot transition from kmix to something else, so if
        // the user had a custom shortcut set for kmix those would get lost.
        // To avoid this we hijack kmix name and actions. Entirely mental but
        // best we can do to not cause annoyance for the user.
        // The display name actually is updated to whatever registered last
        // though, so as far as user visible strings go we should be fine.
        // As of 2015-07-21:
        //   componentName: kmix
        //   actions: increase_volume, decrease_volume, mute
        name: "kmix"
        displayName: main.displayName
        GlobalAction {
            objectName: "increase_volume"
            text: i18n("Increase Volume")
            shortcut: Qt.Key_VolumeUp
            onTriggered: increaseVolume()
        }
        GlobalAction {
            objectName: "decrease_volume"
            text: i18n("Decrease Volume")
            shortcut: Qt.Key_VolumeDown
            onTriggered: decreaseVolume()
        }
        GlobalAction {
            objectName: "mute"
            text: i18n("Mute")
            shortcut: Qt.Key_VolumeMute
            onTriggered: muteVolume()
        }
    }

    VolumeOSD {
        id: osd
    }

    PlasmaExtras.ScrollArea {
        id: scrollView;

        anchors {
            fill: parent
            rightMargin: 16
        }

        ColumnLayout {
            property int maximumWidth: scrollView.viewport.width
            width: maximumWidth
            Layout.maximumWidth: maximumWidth

            Header {
                Layout.fillWidth: true
                visible: sinkView.count > 0
                text: i18n("Playback Devices")
            }
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

            Header {
                Layout.fillWidth: true
                visible: sourceView.count > 0
                text: i18n("Capture Devices")
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
