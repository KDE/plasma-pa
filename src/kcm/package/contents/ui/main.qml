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

import QtQuick 2.1
import QtQuick.Controls 1.3

import org.kde.kcm 1.0
import org.kde.plasma.core 2.0 as PlasmaCore /* for units.gridUnit */
import org.kde.kirigami 2.5 as Kirigami
import org.kde.plasma.private.volume 0.1

Kirigami.Page {
    title: kcm.name
    property int wheelDelta: 0
    property QtObject sinkModel: SinkModel { }
    property QtObject sourceModel: SourceModel { }
    ConfigModule.quickHelp: i18nd("kcm_pulseaudio", "This module allows configuring the Pulseaudio sound subsystem.")

    contentItem: MouseArea {
        acceptedButtons: Qt.NoButton
        implicitWidth: units.gridUnit * 30
        implicitHeight: units.gridUnit * 30

        onWheel: {
            if (tabView.childAt(wheel.x, wheel.y).objectName != "tabbar") {
                wheel.accepted = false;
                return;
            }
            var delta = wheel.angleDelta.y || wheel.angleDelta.x;
            wheelDelta += delta;
            // Magic number 120 for common "one click"
            // See: http://qt-project.org/doc/qt-5/qml-qtquick-wheelevent.html#angleDelta-prop
            while (wheelDelta >= 120) {
                wheelDelta -= 120;
                tabView.currentIndex = Math.max(0, tabView.currentIndex - 1);
            }
            while (wheelDelta <= -120) {
                wheelDelta += 120;
                tabView.currentIndex = Math.min(tabView.count - 1, tabView.currentIndex + 1);
            }
        }

        TabView {
            id: tabView
            anchors.fill: parent

            Tab {
                title: i18ndc("kcm_pulseaudio", "@title:tab", "Devices")
                Devices {}
            }
            Tab {
                title: i18ndc("kcm_pulseaudio", "@title:tab", "Applications")
                Applications {}
            }
            Tab {
                title: i18ndc("kcm_pulseaudio", "@title:tab", "Advanced")
                Advanced {}
            }
        }
    }
}
