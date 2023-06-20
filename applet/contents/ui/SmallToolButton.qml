/*
    SPDX-FileCopyrightText: 2017 Chris Holland <zrenfire@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick 2.0
import QtQuick.Layouts 1.0
import org.kde.kirigami 2.20 as Kirigami
import org.kde.plasma.components 3.0 as PlasmaComponents3

PlasmaComponents3.ToolButton {
    id: smallToolButton
    readonly property int size: Math.ceil(Kirigami.Units.iconSizes.small + Kirigami.Units.smallSpacing * 2)
    implicitWidth: size
    implicitHeight: size

    display: PlasmaComponents3.AbstractButton.IconOnly
}
