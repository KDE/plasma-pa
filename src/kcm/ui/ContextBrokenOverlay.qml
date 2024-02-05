// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2024 Harald Sitter <sitter@kde.org>

import QtQuick
import QtQuick.Controls

import org.kde.kirigami as Kirigami
import org.kde.plasma.private.volume

// WARNING only construct this at runtime. At loading time we don't have a window and the parenting gets messed up.
Loader {
    id: loader

    // Don't flicker the error on startup. Only show it after the startup timer triggers.
    property bool __startupDelayTriggered: false
    readonly property Component __overlayComponent: Pane {
        visible: true
        parent: loader.Overlay.overlay
        anchors.fill: parent
        enabled: true

        Kirigami.PlaceholderMessage {
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

    Timer {
        id: startupDelay
        interval: 500
        running: true
        onTriggered: loader.__startupDelayTriggered = true
    }

    sourceComponent: {
        if (__startupDelayTriggered && Context.state !== Context.State.Ready && Overlay.overlay) {
            return __overlayComponent
        }
        return null
    }
}
