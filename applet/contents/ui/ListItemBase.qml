/*
    Copyright 2014-2015 Harald Sitter <sitter@kde.org>

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

import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

import org.kde.kquickcontrolsaddons 2.0
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.extras 2.0 as PlasmaExtras

PlasmaComponents.ListItem {
    id: item

    property alias label: textLabel.text
    property alias icon: clientIcon.source

    function setVolume(volume) {
        if (volume > 0 && Muted) {
            Muted = false;
        }
        PulseObject.volume = volume
    }

    anchors {
        left: parent.left;
        right: parent.right;
    }

    ColumnLayout {
        property int maximumWidth: parent.width
        width: maximumWidth
        Layout.maximumWidth: maximumWidth

        RowLayout {
            Layout.fillWidth: true
            spacing: units.smallSpacing

            PlasmaCore.IconItem {
                id: clientIcon
                visible: valid
                Layout.alignment: Qt.AlignHCenter
                width: height
                height: column.height * 0.75
            }

            ColumnLayout {
                id: column

                PlasmaExtras.Heading {
                    id :textLabel
                    Layout.fillWidth: true
                    level: 5
                    opacity: 0.6
                    wrapMode: Text.NoWrap
                    elide: Text.ElideRight
                }

                RowLayout {
                    VolumeIcon {
                        Layout.maximumHeight: slider.height * 0.75
                        Layout.maximumWidth: slider.height* 0.75
                        volume: PulseObject.volume
                        muted: PulseObject.muted

                        MouseArea {
                            anchors.fill: parent
                            onPressed: PulseObject.muted = !PulseObject.muted
                        }
                    }

                    PlasmaComponents.Slider {
                        id: slider

                        // Helper properties to allow async slider updates.
                        // While we are sliding we must not react to value updates
                        // as otherwise we can easily end up in a loop where value
                        // changes trigger volume changes trigger value changes.
                        property int volume: PulseObject.volume
                        property bool ignoreValueChange: false

                        Layout.fillWidth: true
                        minimumValue: 0
                        // FIXME: I do wonder if exposing max through the model would be useful at all
                        maximumValue: 65536
                        stepSize: maximumValue / 100
                        visible: PulseObject.hasVolume
                        enabled: {
                            if (typeof PulseObject.volumeWritable === 'undefined') {
                                return !PulseObject.muted
                            }
                            return PulseObject.volumeWritable && !PulseObject.muted
                        }

                        onVolumeChanged: {
                            ignoreValueChange = true;
                            value = PulseObject.volume;
                            ignoreValueChange = false;
                        }

                        onValueChanged: {
                            if (!ignoreValueChange) {
                                setVolume(value);

                                if (!pressed) {
                                    updateTimer.restart();
                                }
                            }
                        }

                        onPressedChanged: {
                            if (!pressed) {
                                // Make sure to sync the volume once the button was
                                // released.
                                // Otherwise it might be that the slider is at v10
                                // whereas PA rejected the volume change and is
                                // still at v15 (e.g.).
                                updateTimer.restart();
                            }
                        }

                        Timer {
                            id: updateTimer
                            interval: 200
                            onTriggered: slider.value = PulseObject.volume
                        }
                    }
                    PlasmaComponents.Label {
                        id: percentText
                        Layout.alignment: Qt.AlignHCenter
                        Layout.minimumWidth: referenceText.width
                        horizontalAlignment: Qt.AlignRight
                        text: i18nc("volume percentage", "%1%", Math.floor(slider.value / slider.maximumValue * 100.0))
                    }
                }
            }
        }
    }

    PlasmaComponents.Label {
        id: referenceText
        visible: false
        text: i18nc("only used for sizing, should be widest possible string", "100%")
    }
}
