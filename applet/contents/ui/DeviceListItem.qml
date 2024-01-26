/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick

import org.kde.plasma.private.volume


ListItemBase {
    readonly property var currentPort: model.Ports[model.ActivePortIndex]
    readonly property bool muted: model.Muted
    readonly property int activePortIndex: model.ActivePortIndex

    fullNameToShowOnHover: ListView.view.count === 1 ? model.Description : ""

    function nodeName() {
        const nodeNick = model.PulseProperties["node.nick"]
        if (nodeNick) {
            return nodeNick
        }

        if (model.Description) {
            return model.Description
        }

        if (model.Name) {
            return model.Name
        }

        return i18n("Device name not found");
    }

    draggable: false
    label: {
        // >1 entry -> use unique-ish names (e.g. 'Konqi Headset 5')
        if (ListView.view.count > 1) {
            return nodeName()
        }

        // 1 entry -> use the ambiguous port name (e.g. 'Microphone')
        if (currentPort?.description) {
            return currentPort.description
        }

        return nodeName()
    }

    onActivePortIndexChanged: {
        if (type === "sink" && globalMute && !model.Muted) {
            model.Muted = true;
        }
    }

    onMutedChanged: {
        if (type === "sink" && globalMute && !model.Muted) {
            config.globalMute = false;
            config.globalMuteDevices = [];
            config.save();
        }
    }
}
