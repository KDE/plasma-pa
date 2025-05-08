/*
    SPDX-FileCopyrightText: 2017 Chris Holland <zrenfire@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.plasma.components as PlasmaComponents3

PlasmaComponents3.ToolButton {
    id: smallToolButton
    readonly property int iconSize: Kirigami.Units.iconSizes.small
    icon.width: iconSize
    icon.height: iconSize

    display: PlasmaComponents3.AbstractButton.IconOnly
}
