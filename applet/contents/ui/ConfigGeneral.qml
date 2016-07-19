/*
    Copyright 2016 David Rosca <nowrep@gmail.com>

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
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.0

import org.kde.plasma.private.volume 0.1

Item {
    property alias cfg_maximumVolume: maximumVolume.value
    property alias cfg_volumeStep: volumeStep.value
    property alias cfg_volumeFeedback: volumeFeedback.checked

    ColumnLayout {
        Layout.fillWidth: true

        GroupBox {
            Layout.fillWidth: true
            flat: true
            title: i18n("Volume")

            GridLayout {
                columns: 2
                Layout.fillWidth: true

                Label {
                    Layout.alignment: Qt.AlignRight
                    text: i18n("Maximum volume:")
                }

                SpinBox {
                    id: maximumVolume
                    minimumValue: 100
                    maximumValue: 150
                    stepSize: 1
                    suffix: i18n("%")
                }

                Label {
                    Layout.alignment: Qt.AlignRight
                    text: i18n("Volume step:")
                }

                SpinBox {
                    id: volumeStep
                    minimumValue: 1
                    maximumValue: 100
                    stepSize: 1
                    suffix: i18n("%")
                }
            }
        }

        GroupBox {
            Layout.fillWidth: true
            flat: true
            title: i18n("Behavior")

            ColumnLayout {
                CheckBox {
                    id: volumeFeedback
                    text: i18n("Volume feedback")
                    enabled: feedback.valid
                }
            }
        }
    }

    VolumeFeedback {
        id: feedback
    }
}
