// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2014-2024 Harald Sitter <sitter@kde.org>
// SPDX-FileCopyrightText: 2016 David Rosca <nowrep@gmail.com>
// SPDX-FileCopyrightText: 2019 Sefa Eyeoglu <contact@scrumplex.net>
// SPDX-FileCopyrightText: 2020 Nicolas Fella <nicolas.fella@gmx.de>
// SPDX-FileCopyrightText: 2021 Ismael Asensio <isma.af@gmail.com>

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
    id: page

    required property var config
    // These are the properties we will override and draw values from, not the UI choices. Those are controlled by the kcfg.
    readonly property list<string> possibleProperties: ['node.description',
                                                        'device.description',
                                                        'node.nick',
                                                        'device.name',
                                                        'alsa.card_name',
                                                        'alsa.long_card_name']

    title: i18nc("@title rename audio devices", "Rename Devices")
    implicitHeight: Kirigami.Units.gridUnit * 28
    implicitWidth: Kirigami.Units.gridUnit * 28

    actions: [
        Kirigami.Action {
            icon.source: "dialog-ok-apply-symbolic"
            text: i18nc("@action save changes", "Save")
            visible: saver.dirty
            onTriggered: saver.saveChanges()
        }
    ]

    DeviceRenameModel {
        id: sinkRenameModel
        sourceModel: SinkModel {}
    }

    DeviceRenameModel {
        id: sourceRenameModel
        sourceModel: SourceModel {}
    }

    DeviceRenameSaver {
        id: saver
        models: [sinkRenameModel, sourceRenameModel]
    }

    headerPaddingEnabled: false // Let the InlineMessages touch the edges
    header: ColumnLayout {
        spacing: Kirigami.Units.smallSpacing

        Kirigami.InlineMessage {
            Layout.fillWidth: true
            visible: true
            type: Kirigami.MessageType.Information
            position: Kirigami.InlineMessage.Position.Header
            text: i18nc("@info", "Saving changes will restart audio services. Apps playing audio will experience interruptions and may need to be restarted.")
        }

        Kirigami.InlineMessage {
            Layout.fillWidth: true
            visible: saver.error !== ''
            type: Kirigami.MessageType.Error
            position: Kirigami.InlineMessage.Position.Header
            text: saver.error

            actions: [
                Kirigami.Action {
                    text: i18nc("@action", "Report Bug")
                    icon.name: "tools-report-bug-symbolic"
                    onTriggered: Qt.openUrlExternally("https://bugs.kde.org/enter_bug.cgi?product=plasma-pa")
                }
            ]
        }
    }

    Kirigami.PlaceholderMessage {
        anchors.centerIn: parent
        width: parent.width - (Kirigami.Units.largeSpacing * 4)
        visible: page.state === "busy"
        text: i18nc("@info:status reloading the backing data", "Reloading")
    }

    component RenameDelegate: ColumnLayout {
        id: delegate

        readonly property var pulseProperties: PulseProperties
        readonly property var hasOverride: HasOverride
        readonly property var hadOverride: HadOverride
        readonly property string description: Description
        readonly property string name: Name

        RowLayout {
            id: root
            spacing: Kirigami.Units.smallSpacing
            Layout.fillWidth: true

            readonly property string initialText: {
                if (delegate.description) {
                    return delegate.description
                }

                if (delegate.name) {
                    return delegate.name
                }

                console.warn("Failed to find initialText property.")
                return ''
            }

            Kirigami.ActionTextField {
                id: textField
                Layout.fillWidth: true

                property bool dirty: false

                placeholderText: {
                    if (delegate.hadOverride) {
                        return i18nc("place holder text, %1 is a device name", "%1 (click 'Save' to apply)", root.initialText)
                    }
                    return root.initialText
                }

                onEditingFinished: {
                    if (text === root.initialText) {
                        return
                    }
                    let properties = pulseProperties
                    for (const key of page.possibleProperties) {
                        properties[key] = text
                    }
                    PulseProperties = properties
                }

                onTextEdited: {
                    dirty = text !== root.initialText
                }

                rightActions: [
                    Kirigami.Action {
                        visible: textField.dirty
                        text: i18nc("@action apply currently editing test", "Apply")
                        icon.name: "dialog-ok-apply"
                        onTriggered: textField.editingFinished()
                    },
                    Kirigami.Action {
                        visible: delegate.hasOverride
                        text: i18nc("@action reset device name to default", "Reset")
                        icon.name: "edit-reset-symbolic"
                        onTriggered: {
                            const model = delegate.ListView.view.model
                            model.reset(model.index(index, 0))

                            textField.reset()
                            textField.placeholderText = i18nc("@label placeholder text when we don't know the default value of a device yet", "DEFAULT")
                        }
                    }
                ]

                function reset() {
                    clear()
                    if (delegate.hasOverride) {
                        // the current value is already an override. use it as text rather than placeholder
                        // There is a bit of a problem because we can't rightly tell what the original property
                        // was, so we can't keep placeholder and text 100% correct, but we can at least
                        // indicate if this is an override or not.
                        text = delegate.pulseProperties['node.description']
                    } else {
                        text = ""
                    }
                }

                Component.onCompleted: reset()
            }
        }
    }

    view: Flickable {
        id: flickable

        contentWidth: column.width
        contentHeight: column.height
        clip: true
        visible: page.state === ""

        ColumnLayout {
            id: column
            width: flickable.width
            spacing: Kirigami.Units.smallSpacing

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
                model: sinkRenameModel

                delegate: RenameDelegate {
                    width: ListView.view.width
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
                model: sourceRenameModel
                delegate: RenameDelegate {}
            }
        }
    }

    states: [
        State { name: "busy"; when: saver.busy },
        State { name: "" }
    ]
}
