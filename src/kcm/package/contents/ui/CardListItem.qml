/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

import org.kde.kirigami 2.12 as Kirigami
import org.kde.plasma.private.volume 0.1

ColumnLayout {
    id: delegate
    width: parent.width

    RowLayout {
        Layout.fillWidth: true
        spacing: Kirigami.Units.largeSpacing

        Kirigami.Icon {
            Layout.alignment: Qt.AlignHCenter
            width: height
            height: Kirigami.Units.iconSizes.medium
            source: IconName || "audio-card"
        }

        Label {
            id: nameLabel
            Layout.fillWidth: true
            text: Properties["device.description"] || Name
            elide: Text.ElideRight

            // Show tooltip on hover when text elided
            ToolTip {
                text: parent.text
                visible: parent.truncated && inputTextMouseArea.containsMouse
            }
            MouseArea {
                id: inputTextMouseArea
                anchors.fill: parent
                hoverEnabled: true
            }
        }

        Label {
            id: profileLabel
            text: i18ndc("kcm_pulseaudio", "@label", "Profile:")
        }

        ComboBox {
            Layout.preferredWidth: Kirigami.Units.gridUnit * 12
            model: Profiles.filter(function (profile) {
                return profile.availability === Profile.Available;
            })
            // NOTE: model resets (i.e. profiles property changes) will reset
            // the currentIndex, so force it to be set on model changes, otherwise
            // it would eventually become 0 when it shouldn't be.
            onModelChanged: currentIndex = model.indexOf(Profiles[ActiveProfileIndex])
            textRole: "description"
            onActivated: ActiveProfileIndex = Profiles.indexOf(model[index])
        }
    }

    Kirigami.Separator {
        visible: (delegate.ListView.view.count != 0) && (delegate.ListView.view.count != (index + 1))
        Layout.topMargin: Kirigami.Units.smallSpacing
        Layout.fillWidth: true
    }
}
