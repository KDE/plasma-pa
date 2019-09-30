/*
    Copyright 2014-2015 Harald Sitter <sitter@kde.org>
    Copyright 2019 Sefa Eyeoglu <contact@scrumplex.net>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of
    the License or (at your option) version 3 or any later version
    accepted by the membership of KDE e.V. (or its successor approved
    by the membership of KDE e.V.), which shall act as a proxy
    defined in Section 14 of version 3 of the license.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

import org.kde.kirigami 2.5 as Kirigami
import org.kde.plasma.private.volume 0.1
import "../code/icon.js" as Icon

ColumnLayout {
    id: delegate
    width: parent.width

    property bool isPlayback: type.substring(0, 4) == "sink"

    readonly property var currentPort: Ports[ActivePortIndex]

    RowLayout {
        Kirigami.Icon {
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredHeight: delegateColumn.height * 0.75
            Layout.preferredWidth: Layout.preferredHeight
            source: Icon.formFactorIcon(FormFactor) || IconName || "audio-card"
        }

        ColumnLayout {
            id: delegateColumn
            Layout.fillWidth: true

            RowLayout {
                Label {
                    id: inputText
                    Layout.fillWidth: true
                    elide: Text.ElideRight
                    text: !currentPort ? Description : i18ndc("kcm_pulseaudio", "label of device items", "%1 (%2)", currentPort.description, Description)
                }

                Label {
                    visible: portbox.count > 1
                    text: i18nd("kcm_pulseaudio", "Port:")
                }

                ComboBox {
                    id: portbox
                    visible: portbox.count > 1
                    readonly property var ports: Ports
                    onModelChanged: currentIndex = ActivePortIndex
                    currentIndex: ActivePortIndex
                    onActivated: ActivePortIndex = index

                    onPortsChanged: {
                        var items = [];
                        for (var i = 0; i < ports.length; ++i) {
                            var port = ports[i];
                            var text = port.description;
                            if (port.availability == Port.Unavailable) {
                                if (port.name == "analog-output-speaker" || port.name == "analog-input-microphone-internal") {
                                    text += i18ndc("kcm_pulseaudio", "Port is unavailable", " (unavailable)");
                                } else {
                                    text += i18ndc("kcm_pulseaudio", "Port is unplugged", " (unplugged)");
                                }
                            }
                            items.push(text);
                        }
                        model = items;
                    }
                }

                Button {
                    text: i18n("Default Device")
                    icon.name: "favorite"
                    visible: delegate.ListView.view.count > 1
                    checkable: true
                    checked: Default
                    onClicked: Default = true;
                }
            }

            RowLayout {
                MuteButton {
                    Layout.topMargin: -(height - icon.height) / 2
                    muted: Muted
                    onCheckedChanged: Muted = checked
                }

                VolumeSlider {}
            }
        }
    }

    ListItemSeperator { view: delegate.ListView.view }
}
