import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

import org.kde.kquickcontrolsaddons 2.0

import org.kde.plasma.private.volume 0.1

ColumnLayout {
    id: delegate

    width: parent.width

    RowLayout {
        Layout.fillWidth: true
        spacing: units.gridUnit

        ClientIcon {
            id: clientIcon

            height: delegateColumn.height / 3 * 1.5
            width: height
            client: PulseObject.client
        }

        ColumnLayout {
            id: delegateColumn
            Layout.fillWidth: true

            RowLayout {
                Label {
                    id: inputText
                    Layout.alignment: Qt.AlignBottom
                    Layout.fillWidth: true
                    text: PulseObject.client.name + ": " + PulseObject.name
                }

                MuteButton {
                    muted: PulseObject.muted
                    onMutedChanged: PulseObject.muted = muted
                }
            }

            VolumeSlider {}
        }
    }

    ListItemSeperator { view: delegate.ListView.view }
}
