// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2026 Harald Sitter <sitter@kde.org>

import QtQuick

import org.kde.kirigami as Kirigami
import org.kde.ki18n
import org.kde.kworkspace as KWorkspace

import org.kde.plasma.private.volume

Kirigami.InlineMessage {
    id: root

    showCloseButton: false
    type: Kirigami.MessageType.Error
    visible: false // set by timer as necessary

    // Default device resolution may fail transiently when devices disappear and reappear in quick order.
    // We only display warnings about this if the problem persists for a few seconds.
    readonly property Timer timer: Timer {
        readonly property Connections connections: Connections {
            target: Server
            function onDefaultSinkChanged() {
                root.timer.restart()
            }
            function onDefaultSourceChanged() {
                root.timer.restart()
            }
        }

        interval: 4000
        running: true // start initially to probe the initial state in case the devices never change (from null)
        repeat: false
        onTriggered: root.visible = !Server.defaultSink || !Server.defaultSource
    }

    text: KI18n.xi18nc("@info", "Something went wrong while trying to determine the default audio devices. Please <link url='https://bugs.kde.org/enter_bug.cgi?product=plasmashell&component=Audio%20Volume%20widget'>open a bug report</link> with the output of <command>journalctl --user</command>. To resolve the situation you may need to restart the system.")
    onLinkActivated: link => Qt.openUrlExternally(link)

    actions: [
        Kirigami.Action {
            property KWorkspace.SessionManagement sessionManagement: KWorkspace.SessionManagement {}
            visible: sessionManagement.canReboot
            text: KI18n.i18nc("@action:button reboot the system", "Restart")
            onTriggered: sessionManagement.requestReboot()
        }
    ]
}
