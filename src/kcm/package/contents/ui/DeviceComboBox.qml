/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>
    SPDX-FileCopyrightText: 2016 David Rosca <nowrep@gmail.com>
    SPDX-FileCopyrightText: 2020 David Redondo <kde@david-redondo.de>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick 2.14
import QtQuick.Controls 2.14

ComboBox {
    textRole: "Description"
    valueRole: "Index"

    onActivated: DeviceIndex = currentValue

    Component.onCompleted: currentIndex = indexOfValue(DeviceIndex)
}
