/*
    SPDX-FileCopyrightText: 2016 David Rosca <nowrep@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick 2.5
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5 as QQC2

import org.kde.kirigami 2.5 as Kirigami

import org.kde.plasma.private.volume 0.1

Kirigami.FormLayout {
    property alias cfg_volumeStep: volumeStep.value
    property alias cfg_volumeFeedback: volumeFeedback.checked
    property alias cfg_volumeOsd: volumeOsd.checked
    property alias cfg_micOsd: micOsd.checked
    property alias cfg_muteOsd: muteOsd.checked
    property alias cfg_outputChangeOsd: outputChangeOsd.checked

    VolumeFeedback {
        id: feedback
    }


    QQC2.SpinBox {
        id: volumeStep

        Kirigami.FormData.label: i18n("Volume step:")

        from: 1
        to: 100
        stepSize: 1
        editable: true
        textFromValue: function(value) {
            return value + "%";
        }
        valueFromText: function(text) {
            return parseInt(text);
        }
    }


    Item {
        Kirigami.FormData.isSection: true
    }


    QQC2.CheckBox {
        id: volumeFeedback

        Kirigami.FormData.label: i18n("Play audio feedback for changes to:")

        text: i18n("Audio volume")
        enabled: feedback.valid
    }

    Item {
        Kirigami.FormData.isSection: true
    }

    QQC2.CheckBox {
        id: volumeOsd
        Kirigami.FormData.label: i18n("Show visual feedback for changes to:")
        text: i18n("Audio volume")
    }

    QQC2.CheckBox {
        id: micOsd
        text: i18n("Microphone sensitivity")
    }

    QQC2.CheckBox {
        id: muteOsd
        text: i18n("Mute state")
    }

    QQC2.CheckBox {
        id: outputChangeOsd
        text: i18n("Default output device")
    }
}
