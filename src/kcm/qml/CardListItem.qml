import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

import org.kde.kquickcontrolsaddons 2.0

ColumnLayout {
    id: item

    property int uniformLabelWidth: Math.max(profileLabel.contentWidth, portLabel.contentWidth)

    width: parent.width

    RowLayout {
        QIconItem {
            id: clientIcon
            Layout.alignment: Qt.AlignHCenter
            width: height
            height: nameLabel.contentHeight
            icon: Properties['device.icon_name'] ? Properties['device.icon_name'] : 'audio-card'
        }

        Label {
            id: nameLabel
            Layout.fillWidth: true
            text: Properties['device.description'] ? Properties['device.description'] : Name
            elide: "ElideRight"
        }
    }

    RowLayout {
        Label {
            id: profileLabel
            Layout.minimumWidth: item.uniformLabelWidth
            Layout.maximumWidth: Layout.minimumWidth
            // FIXME: i18n
            text: "Profile:"
            font.bold: true
        }
        ComboBox {
            Layout.fillWidth: true
            model: Profiles
            textRole: "description"
            currentIndex: ActiveProfileIndex
            onCurrentIndexChanged: {
                // Get name.
                textRole = "name";
                var name = currentText;
                textRole = "description";
                pulseContext.setCardProfile(Index, name);
            }
        }
    }

    RowLayout {
        Label {
            id: portLabel
            Layout.minimumWidth: item.uniformLabelWidth
            Layout.maximumWidth: Layout.minimumWidth
            // FIXME: i18n
            text: "Ports:"
            font.bold: true
        }
        ComboBox {
            Layout.fillWidth: true
            model: Ports
            textRole: "description"
        }
    }

    Rectangle {
        // FIXME: needs kcolorscheme backing
        color: "grey"
        Layout.fillWidth: true
        height: 1
    }
}
