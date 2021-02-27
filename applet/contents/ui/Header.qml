/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick 2.0
import org.kde.plasma.components 2.0 as PlasmaComponents // for ListItem
import org.kde.plasma.components 3.0 as PlasmaComponents3
import org.kde.plasma.core 2.0 as PlasmaCore

PlasmaComponents.ListItem {
    property alias text: label.text

    height: PlasmaCore.Units.gridUnit * 1.5
    sectionDelegate: true

    PlasmaComponents3.Label {
        id: label
        anchors.centerIn: parent
        font.weight: Font.DemiBold
    }
}
