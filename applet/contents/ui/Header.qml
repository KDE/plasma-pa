import QtQuick 2.0
import org.kde.plasma.components 2.0 as PlasmaComponents

PlasmaComponents.ListItem {
    property alias text: label.text

    height: label.height + units.gridUnit
    sectionDelegate: true

    PlasmaComponents.Label {
        id: label
        anchors.centerIn: parent
        height: paintedHeight
        font.weight: Font.DemiBold
    }
}
