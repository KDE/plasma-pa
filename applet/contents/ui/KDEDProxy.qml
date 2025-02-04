// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2025 Harald Sitter <sitter@kde.org>

import QtQuick
import org.kde.plasma.private.volume

QtObject {
    id: kded

    readonly property DBusInterface iface: DBusInterface {
        name: "org.kde.kded6"
        path: "/modules/audioshortcutsservice"
        iface: "org.kde.plasma.audioshortcuts"
        proxy: kded
    }

    property bool contextConnected
    property bool contextAutoConnecting

    function reconnectContextAsync() {
        return new Promise((resolve, reject) => { iface.asyncCall("reconnectContext", {}, resolve, reject) })
    }
}
