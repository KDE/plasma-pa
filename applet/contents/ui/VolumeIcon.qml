import QtQuick 2.0
import QtQuick.Layouts 1.0

import org.kde.plasma.core 2.0 as PlasmaCore

import "../code/icon.js" as Icon

PlasmaCore.SvgItem {
    property int volume
    property bool muted

    svg: PlasmaCore.Svg { imagePath: "icons/audio" }
    elementId: Icon.name(volume, muted)
}
