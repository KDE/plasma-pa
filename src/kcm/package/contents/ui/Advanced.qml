/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.2

import org.kde.plasma.private.volume 0.1
import org.kde.kcm 1.3 as KCM
import org.kde.kirigami 2.5 as Kirigami

KCM.SimpleKCM {

    title: i18n("Configure")

    ColumnLayout {

        Label {
            Layout.alignment: Qt.AlignHCenter
            text: i18nd("kcm_pulseaudio", "Advanced Output Configuration")
            visible: moduleManager.settingsSupported
            font.weight: Font.DemiBold
        }

        ModuleManager {
            id: moduleManager
        }

        CheckBox {
            Layout.fillWidth: true
            Layout.topMargin: Kirigami.Units.smallSpacing
            Layout.leftMargin: Kirigami.Units.gridUnit / 2
            Layout.rightMargin: Kirigami.Units.gridUnit / 2
            text: i18nd("kcm_pulseaudio", "Add virtual output device for simultaneous output on all local sound cards")
            checked: moduleManager.combineSinks
            onToggled: moduleManager.combineSinks = checked;
            enabled: moduleManager.configModuleLoaded
            visible: moduleManager.settingsSupported
        }

        CheckBox {
            Layout.fillWidth: true
            Layout.leftMargin: Kirigami.Units.gridUnit / 2
            Layout.rightMargin: Kirigami.Units.gridUnit / 2
            text: i18nd("kcm_pulseaudio", "Automatically switch all running streams when a new output becomes available")
            checked: moduleManager.switchOnConnect
            onToggled: moduleManager.switchOnConnect = checked;
            enabled: moduleManager.configModuleLoaded
            visible: moduleManager.settingsSupported
        }

        Label {
            Layout.alignment: Qt.AlignHCenter
            enabled: false
            font.italic: true
            text: i18nd("kcm_pulseaudio", "Requires %1 PulseAudio module", moduleManager.configModuleName)
            visible: moduleManager.settingsSupported && !moduleManager.configModuleLoaded
        }
    }
}
