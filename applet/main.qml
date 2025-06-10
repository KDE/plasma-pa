/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.Layouts

import org.kde.plasma.core as PlasmaCore
import org.kde.ksvg as KSvg
import org.kde.kirigami as Kirigami
import org.kde.kitemmodels as KItemModels
import org.kde.plasma.components as PC3
import org.kde.plasma.extras as PlasmaExtras
import org.kde.plasma.plasmoid

import org.kde.kcmutils as KCMUtils
import org.kde.config as KConfig

import org.kde.plasma.private.volume

PlasmoidItem {
    id: main

    GlobalConfig {
        id: config
    }

    height: Kirigami.Units.gridUnit * 21
    width: Kirigami.Units.gridUnit * 24

    property bool volumeFeedback: config.audioFeedback
    property bool globalMute: config.globalMute
    property string displayName: i18n("Audio Volume")
    property QtObject draggedStream: null

    property bool showVirtualDevices: Plasmoid.configuration.showVirtualDevices

    readonly property bool inPanel: (Plasmoid.location === PlasmaCore.Types.TopEdge
        || Plasmoid.location === PlasmaCore.Types.RightEdge
        || Plasmoid.location === PlasmaCore.Types.BottomEdge
        || Plasmoid.location === PlasmaCore.Types.LeftEdge)
    readonly property bool compactInPanel: inPanel && !!compactRepresentationItem?.visible

    // DEFAULT_SINK_NAME in module-always-sink.c
    readonly property string dummyOutputName: "auto_null"
    readonly property bool contextIsReady: Context.state === Context.State.Ready
    readonly property string noDevicePlaceholderMessage: {
        if (!contextIsReady) {
            if (!Context.autoConnecting) {
                return i18nc("@label", "Connection to the Sound Service Lost")
            }
            return i18nc("@label", "Connection to the Sound Service Lost")
        }
        return i18n("No output or input devices found")
    }
    readonly property string noDeviceExplanation: {
        if (!contextIsReady && Context.autoConnecting) {
            return i18nc("@info reconnecting to pulseaudio", "Trying to reconnect…")
        }
        return ""
    }

    switchHeight: Kirigami.Units.gridUnit * 8
    switchWidth: Kirigami.Units.gridUnit * 14

    Plasmoid.icon: PreferredDevice.sink && !isDummyOutput(PreferredDevice.sink) ? AudioIcon.forVolume(volumePercent(PreferredDevice.sink.volume), PreferredDevice.sink.muted, "")
                                                                                          : AudioIcon.forVolume(0, true, "")
    toolTipMainText: {
        var sink = PreferredDevice.sink
        if (!sink || isDummyOutput(sink)) {
            return displayName;
        }

        if (sink.muted) {
            return i18n("Audio Muted");
        } else {
            return i18n("Volume at %1%", volumePercent(sink.volume));
        }
    }
    toolTipSubText: {
        let lines = [];

        if (PreferredDevice.sink && paSinkFilterModel.count > 1 && !isDummyOutput(PreferredDevice.sink)) {
            lines.push(nodeName(PreferredDevice.sink))
        }

        if (paSinkFilterModel.count > 0) {
            lines.push(main.globalMute ? i18n("Middle-click to unmute")
                                    : i18n("Middle-click to mute all audio"));
            lines.push(i18n("Scroll to adjust volume"));
        } else {
            lines.push(main.noDevicePlaceholderMessage);
        }
        return lines.join("\n");
    }

    function nodeName(pulseObject) {
        if (pulseObject.description) {
            return pulseObject.description
        }

        if (pulseObject.name) {
            return pulseObject.name
        }

        return i18n("Device name not found")
    }

    function isDummyOutput(output) {
        return output && output.name === dummyOutputName;
    }

    function volumePercent(volume) {
        return Math.round(volume / PulseAudio.NormalVolume * 100.0);
    }

    function playFeedback(sinkIndex) {
        if (!volumeFeedback) {
            return;
        }
        if (sinkIndex == undefined) {
            sinkIndex = PreferredDevice.sink.index;
        }
        feedback.play(sinkIndex);
    }

    // Output devices
    readonly property SinkModel paSinkModel: SinkModel { id: paSinkModel }

    // Input devices
    readonly property SourceModel paSourceModel: SourceModel { id: paSourceModel }

    // Confusingly, Sink Input is what PulseAudio calls streams that send audio to an output device
    readonly property SinkInputModel paSinkInputModel: SinkInputModel { id: paSinkInputModel }

    // Confusingly, Source Output is what PulseAudio calls streams that take audio from an input device
    readonly property SourceOutputModel paSourceOutputModel: SourceOutputModel { id: paSourceOutputModel }

    // active output devices
    readonly property PulseObjectFilterModel paSinkFilterModel: PulseObjectFilterModel {
        id: paSinkFilterModel
        filterOutInactiveDevices: true
        filterVirtualDevices: !main.showVirtualDevices
        sourceModel: paSinkModel
    }

    // active input devices
    readonly property PulseObjectFilterModel paSourceFilterModel: PulseObjectFilterModel {
        id: paSourceFilterModel
        filterOutInactiveDevices: true
        filterVirtualDevices: !main.showVirtualDevices
        sourceModel: paSourceModel
    }

    // non-virtual streams going to output devices
    readonly property PulseObjectFilterModel paSinkInputFilterModel: PulseObjectFilterModel {
        id: paSinkInputFilterModel
        filters: [ { role: "VirtualStream", value: false } ]
        sourceModel: paSinkInputModel
    }

    // non-virtual streams coming from input devices
    readonly property PulseObjectFilterModel paSourceOutputFilterModel: PulseObjectFilterModel {
        id: paSourceOutputFilterModel
        filters: [ { role: "VirtualStream", value: false } ]
        sourceModel: paSourceOutputModel
    }

    readonly property CardModel paCardModel: CardModel {
        id: paCardModel

        function indexOfCardNumber(cardNumber) {
            const indexRole = KItemModels.KRoleNames.role("Index");
            for (let idx = 0; idx < count; ++idx) {
                if (data(index(idx, 0), indexRole) === cardNumber) {
                    return index(idx, 0);
                }
            }
            return index(-1, 0);
        }
    }

    // Only exists because the default CompactRepresentation doesn't expose:
    // - scroll actions
    // - a middle-click action
    // TODO remove once it gains those features.
    compactRepresentation: MouseArea {
        property int wheelDelta: 0
        property bool wasExpanded: false

        anchors.fill: parent
        hoverEnabled: true
        acceptedButtons: Qt.LeftButton | Qt.MiddleButton
        onPressed: mouse => {
            if (mouse.button == Qt.LeftButton) {
                wasExpanded = main.expanded;
            } else if (mouse.button == Qt.MiddleButton) {
                GlobalService.globalMute();
            }
        }
        onClicked: mouse => {
            if (mouse.button == Qt.LeftButton) {
                main.expanded = !wasExpanded;
            }
        }
        onWheel: wheel => {
            const delta = (wheel.inverted ? -1 : 1) * (wheel.angleDelta.y ? wheel.angleDelta.y : -wheel.angleDelta.x);
            if ((wheelDelta > 0 && delta < 0) || (wheelDelta < 0 && delta > 0)) {
                // reset wheelDelta when changing scrolling direction, as there
                // might be a remainder from the previous wheel action.
                wheelDelta = 0;
            }
            wheelDelta += delta;

            // Magic number 120 for common "one click"
            // See: https://qt-project.org/doc/qt-5/qml-qtquick-wheelevent.html#angleDelta-prop
            while (wheelDelta >= 120) {
                wheelDelta -= 120;
                if (wheel.modifiers & Qt.ShiftModifier) {
                    GlobalService.volumeUpSmall();
                } else {
                    GlobalService.volumeUp();
                }
            }
            while (wheelDelta <= -120) {
                wheelDelta += 120;
                if (wheel.modifiers & Qt.ShiftModifier) {
                    GlobalService.volumeDownSmall();
                } else {
                    GlobalService.volumeDown();
                }
            }
        }
        Kirigami.Icon {
            anchors.fill: parent
            source: plasmoid.icon
            active: parent.containsMouse
        }
    }

    VolumeFeedback {
        id: feedback
    }

    fullRepresentation: PlasmaExtras.Representation {
        id: fullRep

        Layout.minimumHeight: (main.inPanel && !main.compactInPanel) ? -1 : main.switchHeight
        Layout.minimumWidth: (main.inPanel && !main.compactInPanel) ? -1 : main.switchWidth

        collapseMarginsHint: true

        KeyNavigation.down: tabBar.currentItem

        property list<string> hiddenTypes: []

        function beginMoveStream(type: /* "sink" | "source" */ string) {
            if (type === "sink") {
                hiddenTypes = ["source"]
            } else if (type === "source") {
                hiddenTypes = ["sink"]
            }
            tabBar.setCurrentIndex(devicesTab.PC3.TabBar.index)
        }

        function endMoveStream() {
            hiddenTypes = []
            tabBar.setCurrentIndex(streamsTab.PC3.TabBar.index)
        }

        Connections {
            target: main
            function onExpandedChanged() : void {
                if (!expanded) {
                    contentView.currentItem.reset()
                }
            }
        }

        header: PlasmaExtras.PlasmoidHeading {
            // Make this toolbar's buttons align vertically with the ones above
            rightPadding: -1
            // Allow tabbar to touch the header's bottom border
            bottomPadding: -bottomInset

            RowLayout {
                anchors.fill: parent
                spacing: Kirigami.Units.smallSpacing

                PC3.TabBar {
                    id: tabBar
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    currentIndex: {
                        switch (plasmoid.configuration.currentTab) {
                        case "devices":
                            return devicesTab.PC3.TabBar.index;
                        case "streams":
                            return streamsTab.PC3.TabBar.index;
                        }
                    }

                    KeyNavigation.right: actionsButton
                    Keys.onDownPressed: event => {
                        contentView.currentItem.contentItem.upperListView.currentIndex = 0
                        event.accepted = false // pass to KeyNavigation, set inside StackView
                    }

                    onCurrentIndexChanged: {
                        switch (currentIndex) {
                        case devicesTab.PC3.TabBar.index:
                            plasmoid.configuration.currentTab = "devices";
                            break;
                        case streamsTab.PC3.TabBar.index:
                            plasmoid.configuration.currentTab = "streams";
                            break;
                        }
                    }

                    PC3.TabButton {
                        id: devicesTab
                        text: i18n("Devices")

                        KeyNavigation.up: fullRep.KeyNavigation.up
                    }

                    PC3.TabButton {
                        id: streamsTab
                        text: i18n("Applications")

                        KeyNavigation.up: fullRep.KeyNavigation.up
                    }
                }

                PC3.ToolButton {
                    id: actionsButton

                    visible: !(plasmoid.containmentDisplayHints & PlasmaCore.Types.ContainmentDrawsPlasmoidHeading)

                    icon.name: "application-menu"
                    display: PC3.AbstractButton.IconOnly
                    checkable: true
                    checked: configMenu.status !== PlasmaExtras.Menu.Closed
                    onToggled: {
                        if (checked) {
                            configMenu.openRelative();
                        } else {
                            configMenu.close();
                        }
                    }
                    KeyNavigation.right: configureButton

                    text: i18nc("@action:button", "More actions")
                    PC3.ToolTip {
                        text: parent.text
                    }
                }

                PlasmaExtras.Menu {
                    id: configMenu
                    visualParent: actionsButton
                    placement: PlasmaExtras.Menu.BottomPosedLeftAlignedPopup
                }

                Instantiator {
                    model: Plasmoid.contextualActions
                    delegate: PlasmaExtras.MenuItem {
                        id: menuItem
                        action: modelData
                    }
                    onObjectAdded: (index, object) => {
                        if (object.action.priority === PlasmaCore.Action.NormalPriority) {
                            configMenu.addMenuItem(object);
                        }
                    }
                }

                PC3.ToolButton {
                    id: configureButton
                    visible: !(plasmoid.containmentDisplayHints & PlasmaCore.Types.ContainmentDrawsPlasmoidHeading)

                    icon.name: "configure"
                    onClicked: plasmoid.internalAction("configure").trigger()

                    Accessible.name: plasmoid.internalAction("configure").text
                    PC3.ToolTip {
                        text: plasmoid.internalAction("configure").text
                    }
                }
            }
        }

        // NOTE: using a StackView instead of a SwipeView partly because
        // SwipeView would never start with the correct initial view when the
        // last saved view was the streams view.
        // We also don't need to be able to swipe between views.
        contentItem: HorizontalStackView {
            id: contentView
            initialItem: plasmoid.configuration.currentTab === "streams" ? streamsView : devicesView
            movementTransitionsEnabled: currentItem !== null
            property TwoPartView deviceViewItem: TwoPartView {
                id: devicesView
                upperModel: paSinkFilterModel
                upperType: "sink"
                lowerModel: paSourceFilterModel
                lowerType: "source"
                iconName: "audio-volume-muted"
                placeholderText: main.noDevicePlaceholderMessage
                explanationText: main.noDeviceExplanation
                helpfulAction: Kirigami.Action {
                    text: i18nc("@action retry connecting to pulseaudio", "Retry")
                    icon.name: "view-refresh-symbolic"
                    onTriggered: Context.reconnectDaemon()
                    // Fun fact: visibility of the helpfulAction inside PlaceholderMessage is linked to enabled, not visible
                    enabled: !Context.autoConnecting && !main.contextIsReady
                }
                upperDelegate: DeviceListItem {
                    width: ListView.view.width
                    type: devicesView.upperType
                    focus: ListView.isCurrentItem
                }
                lowerDelegate: DeviceListItem {
                    width: ListView.view.width
                    type: devicesView.lowerType
                    focus: ListView.isCurrentItem
                }
                HorizontalStackView.onStatusChanged: devicesView.reset()
            }
            // NOTE: Don't unload this while dragging and dropping a stream
            // to a device or else the D&D operation will be cancelled.
            property TwoPartView streamsViewItem: TwoPartView {
                id: streamsView
                upperModel: paSinkInputFilterModel
                upperType: "sink-input"
                lowerModel: paSourceOutputFilterModel
                lowerType: "source-output"
                iconName: "edit-none"
                placeholderText: i18n("No applications playing or recording audio")
                upperDelegate: StreamListItem {
                    width: ListView.view.width
                    type: streamsView.upperType
                    devicesModel: paSinkFilterModel
                    focus: ListView.isCurrentItem
                }
                lowerDelegate: StreamListItem {
                    width: ListView.view.width
                    type: streamsView.lowerType
                    devicesModel: paSourceFilterModel
                    focus: ListView.isCurrentItem
                }
                HorizontalStackView.onStatusChanged: devicesView.reset()
            }
            Connections {
                target: tabBar
                function onCurrentIndexChanged() {
                    if (tabBar.currentItem === devicesTab) {
                        contentView.reverseTransitions = false
                        contentView.replace(devicesView)
                    } else if (tabBar.currentItem === streamsTab) {
                        contentView.reverseTransitions = true
                        contentView.replace(streamsView)
                    }
                }
            }
        }

        component TwoPartView : PC3.ScrollView {
            id: scrollView
            required property PulseObjectFilterModel upperModel
            required property string upperType
            required property Component upperDelegate
            required property PulseObjectFilterModel lowerModel
            required property string lowerType
            required property Component lowerDelegate
            property string iconName: ""
            property string placeholderText: ""
            property string explanationText: ""
            property Kirigami.Action helpfulAction: null

             // HACK: workaround for https://bugreports.qt.io/browse/QTBUG-83890
            PC3.ScrollBar.horizontal.policy: PC3.ScrollBar.AlwaysOff

            function reset() : void {
                upperSection.currentIndex = -1
                lowerSection.currentIndex = -1
            }

            Loader {
                parent: scrollView
                anchors.centerIn: parent
                width: parent.width -  Kirigami.Units.gridUnit * 4
                active: visible
                visible: scrollView.placeholderText.length > 0 && !upperSection.visible && !lowerSection.visible
                sourceComponent: PlasmaExtras.PlaceholderMessage {
                    iconName: scrollView.iconName
                    text: scrollView.placeholderText
                    explanation: scrollView.explanationText
                    helpfulAction: scrollView.helpfulAction
                }
            }
            contentItem: Flickable {
                contentHeight: layout.implicitHeight
                clip: true

                property ListView upperListView: upperSection.visible ? upperSection : lowerSection
                property ListView lowerListView: lowerSection.visible ? lowerSection : upperSection

                ColumnLayout {
                    id: layout
                    width: parent.width
                    spacing: 0

                    PlasmaExtras.ListSectionHeader {
                        id: upperSectionHeader
                        visible: upperSection.visible && text != "" && (upperSection.count > 1 || lowerSection.count > 1)
                        Layout.fillWidth: true
                        topPadding: Kirigami.Units.mediumSpacing
                        leftPadding: !mirrored ? 2 * Kirigami.Units.smallSpacing : padding // extra spacing to align with icon/label/radiobutton
                        rightPadding: mirrored ? 2 * Kirigami.Units.smallSpacing : padding
                        text: {
                            switch (scrollView.upperType) {
                                case "sink":
                                    return i18nc("@title:group listview section header", "Output Devices")
                                case "sink-input":
                                    return i18nc("@title:group listview section header", "Output Streams")
                                default:
                                    return ""
                            }
                        }
                    }

                    ListView {
                        id: upperSection
                        property bool otherViewHasRadioButtons: lowerSection.visible && (lowerSection.count > 1)
                        visible: count && !fullRep.hiddenTypes.includes(scrollView.upperType)
                        interactive: false
                        Layout.fillWidth: true
                        implicitHeight: contentHeight
                        Accessible.name: upperSectionHeader.text
                        Accessible.role: Accessible.List
                        model: scrollView.upperModel
                        delegate: scrollView.upperDelegate
                        focus: visible
                        keyNavigationEnabled: true
                        currentIndex: -1
                        highlight: PlasmaExtras.Highlight {
                            visible: upperSection.activeFocus
                        }
                        highlightMoveDuration: Kirigami.Units.shortDuration

                        // Careful, qml can get confused about the fallback chain between StackView and outside
                        KeyNavigation.up: scrollView.PC3.StackView.status === PC3.StackView.Active ? tabBar : null
                        KeyNavigation.backtab: scrollView.PC3.StackView.status !== PC3.StackView.Active ? null
                                             : (configureButton.visible ? configureButton : tabBar)

                        Keys.onDownPressed: event => {
                            if (currentIndex == count - 1 && lowerSection.visible) {
                                lowerSection.currentIndex = 0;
                            }
                            event.accepted = false; // pass to KeyNavigation
                        }
                        KeyNavigation.down: lowerSection.visible ? lowerSection : raiseMaximumVolumeCheckbox
                    }
                    KSvg.SvgItem {
                        imagePath: "widgets/line"
                        elementId: "horizontal-line"
                        Layout.fillWidth: true
                        Layout.leftMargin: Kirigami.Units.smallSpacing * 2
                        Layout.rightMargin: Layout.leftMargin
                        Layout.topMargin: Kirigami.Units.smallSpacing // a bit smaller as the sliders cause a visual gap
                        visible: upperSection.visible && lowerSection.visible && upperSection.count === 1 && lowerSection.count === 1
                    }
                    PlasmaExtras.ListSectionHeader {
                        id: lowerSectionHeader
                        visible: lowerSection.visible && text != ""  && (upperSection.count > 1 || lowerSection.count > 1)
                        Layout.fillWidth: true
                        topPadding: Kirigami.Units.smallSpacing
                        leftPadding: !mirrored ? 2 * Kirigami.Units.smallSpacing : padding // extra spacing to align with icon/label/radiobutton
                        rightPadding: mirrored ? 2 * Kirigami.Units.smallSpacing : padding
                        text: {
                            switch (scrollView.lowerType) {
                                case "source":
                                    return i18nc("@title:group listview section header", "Input Devices")
                                case "source-output":
                                    return i18nc("@title:group listview section header", "Input Streams")
                                default:
                                    return ""
                            }
                        }
                    }
                    ListView {
                        id: lowerSection
                        property bool otherViewHasRadioButtons: upperSection.visible && (upperSection.count > 1)
                        visible: count && !fullRep.hiddenTypes.includes(scrollView.lowerType)
                        interactive: false
                        Layout.fillWidth: true
                        implicitHeight: contentHeight
                        Accessible.name: lowerSectionHeader.text
                        Accessible.role: Accessible.List
                        model: scrollView.lowerModel
                        delegate: scrollView.lowerDelegate
                        keyNavigationEnabled: true
                        currentIndex: -1
                        highlight: PlasmaExtras.Highlight {
                            visible: lowerSection.activeFocus
                        }
                        highlightMoveDuration: Kirigami.Units.shortDuration

                        Keys.onUpPressed: event => {
                            if (currentIndex == 0 && upperSection.visible) {
                                upperSection.currentIndex = upperSection.count - 1;
                            }
                            event.accepted = false; // pass to KeyNavigation
                        }
                        KeyNavigation.up: upperSection
                        KeyNavigation.down: scrollView.PC3.StackView.status === PC3.StackView.Active ? raiseMaximumVolumeCheckbox : null
                    }
                }
            }
        }

        footer: PlasmaExtras.PlasmoidHeading {
            height: fullRep.header.height
            PC3.Switch {
                id: raiseMaximumVolumeCheckbox
                anchors.left: parent.left
                anchors.leftMargin: Kirigami.Units.smallSpacing
                anchors.verticalCenter: parent.verticalCenter

                checked: config.raiseMaximumVolume

                Accessible.onPressAction: raiseMaximumVolumeCheckbox.toggle()
                Keys.onUpPressed: event => {
                    if (KeyNavigation.up.count > 0) {
                        KeyNavigation.up.currentIndex = KeyNavigation.up.count - 1
                    }
                    event.accepted = false // pass to KeyNavigation, set inside StackView
                }
                Keys.onDownPressed: event => event.accepted = true // don't pass to parent

                text: i18n("Raise maximum volume")

                onToggled: {
                    config.raiseMaximumVolume = checked;
                    config.save();
                }
            }
        }
    }

    Plasmoid.contextualActions: [
        PlasmaCore.Action {
            text: i18n("Force mute all playback devices")
            icon.name: "audio-volume-muted"
            checkable: true
            checked: globalMute
            onTriggered: {
                GlobalService.globalMute();
            }
        },
        PlasmaCore.Action {
            text: i18n("Show virtual devices")
            icon.name: "audio-card"
            checkable: true
            checked: plasmoid.configuration.showVirtualDevices
            onTriggered: Plasmoid.configuration.showVirtualDevices = !Plasmoid.configuration.showVirtualDevices
        }
    ]

    PlasmaCore.Action {
        id: configureAction
        text: i18n("&Configure Audio Devices…")
        icon.name: "configure"
        visible: KConfig.KAuthorized.authorizeControlModule("kcm_pulseaudio")
        onTriggered: KCMUtils.KCMLauncher.openSystemSettings("kcm_pulseaudio")
    }

    Component.onCompleted: {
        MicrophoneIndicator.init();
        Plasmoid.setInternalAction("configure", configureAction);

        // migrate settings if they aren't default
        // this needs to be done per instance of the applet
        if (Plasmoid.configuration.migrated) {
            return;
        }
        if (Plasmoid.configuration.volumeFeedback === false && config.audioFeedback) {
            config.audioFeedback = false;
            config.save();
        }
        if (Plasmoid.configuration.volumeStep && Plasmoid.configuration.volumeStep !== 5 && config.volumeStep === 5) {
            config.volumeStep = Plasmoid.configuration.volumeStep;
            config.save();
        }
        if (Plasmoid.configuration.raiseMaximumVolume === true && !config.raiseMaximumVolume) {
            config.raiseMaximumVolume = true;
            config.save();
        }
        if (Plasmoid.configuration.volumeOsd === false && config.volumeOsd) {
            config.volumeOsd = false;
            config.save();
        }
        if (Plasmoid.configuration.muteOsd === false && config.muteOsd) {
            config.muteOsd = false;
            config.save();
        }
        if (Plasmoid.configuration.micOsd === false && config.microphoneSensitivityOsd) {
            config.microphoneSensitivityOsd = false;
            config.save();
        }
        if (Plasmoid.configuration.globalMute === true && !config.globalMute) {
            config.globalMute = true;
            config.save();
        }
        if (Plasmoid.configuration.globalMuteDevices.length !== 0) {
            for (const device in Plasmoid.configuration.globalMuteDevices) {
                if (!config.globalMuteDevices.includes(device)) {
                    config.globalMuteDevices.push(device);
                }
            }
            config.save();
        }
        Plasmoid.configuration.migrated = true;
    }
}
