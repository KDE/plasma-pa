import QtQuick 2.0
import org.kde.plasma.components 2.0 as Components

Components.ListItem {
    id: header;

   property alias text: headerLabel.text;

    anchors {
        left: parent.left;
        right: parent.right;
    }

    height: headerLabel.height + units.gridUnit;
    sectionDelegate: true;

    Components.Label {
        id: headerLabel;

        anchors {
            horizontalCenter: parent.horizontalCenter;
            verticalCenter: parent.verticalCenter;
        }

        height: paintedHeight;
        font.weight: Font.DemiBold;
    }

}