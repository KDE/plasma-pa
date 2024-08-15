// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2024 Harald Sitter <sitter@kde.org>

import QtQuick
import QtQuick.Controls

import org.kde.kirigami as Kirigami
import org.kde.plasma.private.volume

Kirigami.Page {
    Kirigami.PlaceholderMessage {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.centerIn: parent
        icon.name: "audio-volume-muted"
        text: i18nc("@label", "Connection to the Sound Service Lost")
        explanation: {
            if (Context.autoConnecting) {
                return i18nc("@info reconnecting to pulseaudio", "Trying to reconnect…")
            }
            return ""
        }

        helpfulAction: Kirigami.Action {
            // Enabled governs visibility
            enabled: !Context.autoConnecting
            text: i18nc("@action retry connecting to pulseaudio", "Retry")
            icon.name: "view-refresh-symbolic"
            onTriggered: Context.reconnectDaemon()
        }
    }
}
