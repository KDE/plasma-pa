/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>
    SPDX-FileCopyrightText: 2019 Sefa Eyeoglu <contact@scrumplex.net>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

import org.kde.kirigami 2.12 as Kirigami
import org.kde.plasma.private.volume 0.1

ColumnLayout {
    id: delegate

    property alias deviceModel: deviceComboBox.model
    readonly property bool isEventStream: Name == "sink-input-by-media-role:event"
    property bool isPlayback: type.substring(0, 4) == "sink"

    width: parent.width

    RowLayout {
        Layout.fillWidth: true
        spacing: Kirigami.Units.smallSpacing * 2

        Kirigami.Icon {
            Layout.alignment: Qt.AlignHCenter
            implicitHeight: Kirigami.Units.iconSizes.large
            implicitWidth: implicitHeight
            source: IconName || "unknown"
        }

        ColumnLayout {
            id: delegateColumn
            Layout.fillWidth: true

            RowLayout {
                Label {
                    id: inputText
                    Layout.fillWidth: true
                    text: {
                        if (isEventStream) {
                            return i18nd("kcm_pulseaudio", "Notification Sounds");
                        } else if (Client) {
                            return i18ndc("kcm_pulseaudio", "label of stream items", "%1: %2", Client.name, Name);
                        } else {
                            return Name;
                        }
                    }
                    elide: Text.ElideRight
                }

                DeviceComboBox {
                    id: deviceComboBox
                    Layout.preferredWidth: delegate.width / 3
                    visible: !isEventStream && count > 1
                }
            }

            RowLayout {
                MuteButton {
                    Layout.alignment: Qt.AlignTop
                    Layout.topMargin: -Math.round((height - volumeSlider.height) / 2)
                    muted: Muted
                    onCheckedChanged: Muted = checked
                    toolTipText: inputText.text
                }

                VolumeSlider {
                    id: volumeSlider
                    Layout.alignment: Qt.AlignTop

                    value: Volume
                    onMoved: {
                        Volume = value;
                        Muted = (value === 0);
                    }
                }
            }
        }
    }

    Kirigami.Separator {
        visible: (delegate.ListView.view.count != 0) && (delegate.ListView.view.count != (index + 1))
        Layout.fillWidth: true

        Component.onCompleted: {
            if (isEventStream) {
                visible = Qt.binding(function() {
                    return sinkInputView.count > 0;
                });
            }
        }
    }
}
