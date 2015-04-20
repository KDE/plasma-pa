import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

import org.kde.kquickcontrolsaddons 2.0

ColumnLayout {
    id: delegate

    width: parent.width

    RowLayout {
        QIconItem {
            id: clientIcon
            Layout.alignment: Qt.AlignHCenter
            width: height
            height: nameLabel.contentHeight
            icon: PulseObject.properties['device.icon_name'] ? PulseObject.properties['device.icon_name'] : 'audio-card'
        }

        Label {
            id: nameLabel
            Layout.fillWidth: true
            text: PulseObject.properties['device.description'] ? PulseObject.properties['device.description'] : PulseObject.name
            elide: "ElideRight"
        }
    }

    RowLayout {
        Label {
            id: profileLabel
            text: i18nc("@label", "Profile:")
            font.bold: true
        }
        ComboBox {
            Layout.fillWidth: true
            model: PulseObject.profiles
            // NOTE: model resets (i.e. profiles property changes) will reset
            // the currentIndex, so force it to be set on model changes, otherwise
            // it would eventually become 0 when it shouldn't be.
            onModelChanged: currentIndex = PulseObject.activeProfileIndex
            textRole: "description"
            currentIndex: PulseObject.activeProfileIndex
            onActivated: PulseObject.activeProfileIndex = index
        }
    }

    ListItemSeperator { view: delegate.ListView.view }
}
