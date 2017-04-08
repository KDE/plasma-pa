import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents

PlasmaComponents.ToolButton {
    id: smallToolButton
    property real iconSize: units.iconSizes.small
    property real padding: units.smallSpacing
    property int size: Math.ceil(iconSize + padding * 2)
    implicitWidth: size
    implicitHeight: size
    property alias icon: icon.source

    PlasmaCore.IconItem {
        id: icon
        anchors.fill: parent
        anchors.margins: parent.padding

        // From Plasma's ToolButtonStyle:
        active: parent.hovered
        colorGroup: parent.hovered ? PlasmaCore.Theme.ButtonColorGroup : PlasmaCore.ColorScope.colorGroup
    }
}
