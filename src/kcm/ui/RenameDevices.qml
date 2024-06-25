/*
    SPDX-FileCopyrightText: 2014-2024 Harald Sitter <sitter@kde.org>
    SPDX-FileCopyrightText: 2016 David Rosca <nowrep@gmail.com>
    SPDX-FileCopyrightText: 2019 Sefa Eyeoglu <contact@scrumplex.net>
    SPDX-FileCopyrightText: 2020 Nicolas Fella <nicolas.fella@gmx.de>
    SPDX-FileCopyrightText: 2021 Ismael Asensio <isma.af@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import org.kde.coreaddons as KCoreAddons
import org.kde.kcmutils as KCM
import org.kde.kitemmodels as KItemModels
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.components as KirigamiComponents
import org.kde.plasma.private.volume

KCM.ScrollViewKCM {
    implicitHeight: Kirigami.Units.gridUnit * 28
    implicitWidth: Kirigami.Units.gridUnit * 28

    GlobalConfig {
        id: config
    }

    DeviceRenameModel {
        id: paSinkModel
        sourceModel: SinkModel {}
    }

    SourceModel {
        id: paSourceModel
    }

    CardModel {
        id: paCardModel

        function indexOfCardNumber(cardNumber) {
            for (var idx = 0; idx < count; idx++) {
                if (data(index(idx, 0), KItemModels.KRoleNames.role("Index")) == cardNumber) {
                    return index(idx, 0);
                }
            }
            return index(-1, 0);
        }
    }

    view: Flickable {
        id: flickable

        contentWidth: column.width
        contentHeight: column.height
        clip: true

        ColumnLayout {
            id: column
            width: flickable.width

            Kirigami.ListSectionHeader {
                Layout.fillWidth: true
                visible: sinks.visible
                text: i18nd("kcm_pulseaudio", "Playback Devices")
            }

            ListView {
                id: sinks
                visible: count > 0
                Layout.fillWidth: true
                Layout.leftMargin: Kirigami.Units.largeSpacing
                Layout.rightMargin: Kirigami.Units.largeSpacing
                Layout.topMargin: Kirigami.Units.smallSpacing
                Layout.bottomMargin: Kirigami.Units.smallSpacing
                Layout.preferredHeight: contentHeight
                interactive: false
                spacing: Kirigami.Units.largeSpacing
                model: paSinkModel
                delegate: ColumnLayout {
                    id: delegate

                    width: parent.width

                    property var pulseProperties: PulseProperties

                    component PropertyTextField: RowLayout {
                        id: root
                        required property string key
                        readonly property string initialText: pulseProperties[key]

                        Layout.fillWidth: true

                        TextField {
                            id: textField
                            Layout.fillWidth: true

                            function reset() {
                                text = root.initialText
                            }

                            onEditingFinished: {
                                if (text === root.initialText) {
                                    return
                                }
                                let properties = pulseProperties
                                properties[key] = text
                                PulseProperties = properties
                            }

                            Component.onCompleted: reset()
                        }

                        Button {
                            icon.name: "edit-reset-symbolic"
                            onClicked: textField.reset()
                        }
                    }

                    Kirigami.FormLayout {
                        Layout.fillWidth: true

                        PropertyTextField {
                            key: "node.nick"
                            Kirigami.FormData.label: i18nc("@label", "Nickname:")
                        }
                        PropertyTextField {
                            key: "device.description"
                            Kirigami.FormData.label: i18nc("@label", "Description:")
                        }
                        PropertyTextField {
                            key: "alsa.card_name"
                            Kirigami.FormData.label: i18nc("@label", "ALSA Card Name:")
                        }
                        PropertyTextField {
                            key: "alsa.long_card_name"
                            Kirigami.FormData.label: i18nc("@label", "ALSA Long Card Name:")
                        }
                    }

                    Kirigami.Separator {
                        visible: (delegate.ListView.view.count != 0) && (delegate.ListView.view.count != (index + 1))
                        Layout.topMargin: Kirigami.Units.smallSpacing
                        Layout.fillWidth: true
                    }
                }
            }

            Kirigami.ListSectionHeader {
                Layout.fillWidth: true
                visible: sources.visible
                text: i18nd("kcm_pulseaudio", "Recording Devices")
            }

            ListView {
                id: sources
                visible: count > 0
                Layout.fillWidth: true
                Layout.leftMargin: Kirigami.Units.largeSpacing
                Layout.rightMargin: Kirigami.Units.largeSpacing
                Layout.topMargin: Kirigami.Units.smallSpacing
                Layout.bottomMargin: Kirigami.Units.smallSpacing
                Layout.preferredHeight: contentHeight
                interactive: false
                spacing: Kirigami.Units.largeSpacing
                model: paSourceModel
                // delegate: Kirigami.FormLayout {
                //     TextField {
                //         Kirigami.FormData.label: i18nc("@label", "Device Name:")
                //     }
                // }
            }

            Kirigami.ListSectionHeader {
                Layout.fillWidth: true
                visible: inactiveCards.visible
                text: i18nd("kcm_pulseaudio", "Inactive Cards")
            }

            ListView {
                id: inactiveCards
                visible: count > 0
                Layout.fillWidth: true
                Layout.leftMargin: Kirigami.Units.largeSpacing
                Layout.rightMargin: Kirigami.Units.largeSpacing
                Layout.topMargin: Kirigami.Units.smallSpacing
                Layout.bottomMargin: Kirigami.Units.smallSpacing
                Layout.preferredHeight: contentHeight
                interactive: false
                spacing: Kirigami.Units.largeSpacing
                model: KItemModels.KSortFilterProxyModel {
                    sourceModel: paCardModel
                    filterRowCallback: function(source_row, source_parent) {
                        let idx = sourceModel.index(source_row, 0);
                        let profiles = sourceModel.data(idx, sourceModel.KItemModels.KRoleNames.role("Profiles"))
                        let activeProfileIndex = sourceModel.data(idx, sourceModel.KItemModels.KRoleNames.role("ActiveProfileIndex"))
                        return profiles[activeProfileIndex].name == "off";
                    }
                }
                delegate: CardListItem {
                    comboBoxLabelsVisible: column.comboBoxLabelsVisible
                }
            }
        }
    }
}
