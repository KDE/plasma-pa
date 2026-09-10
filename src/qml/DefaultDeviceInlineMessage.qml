// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2026 Harald Sitter <sitter@kde.org>

import QtQuick

import org.kde.kirigami as Kirigami
import org.kde.ki18n

import org.kde.plasma.private.volume

Kirigami.InlineMessage {
    id: root

    showCloseButton: false
    type: Kirigami.MessageType.Error
    visible: false

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
        repeat: false
        onTriggered: root.visible = !Server.defaultSink || !Server.defaultSource
    }

    text: KI18n.xi18nc("@info", "Something went wrong while trying to determine the default audio devices. Please report a bug with the output of <command>journalctl --user</command>.")

    actions: [
        Kirigami.Action {
            text: KI18n.i18nc("@action:button report a bug", "Report")
            icon.name: "tools-report-bug-symbolic"
            onTriggered: Qt.openUrlExternally("https://bugs.kde.org/enter_bug.cgi?product=plasmashell&component=Audio%20Volume%20widget")
        }
    ]
}
