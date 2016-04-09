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

import org.kde.plasma.private.volume 0.1

TabView {
    property QtObject sinkModel: SinkModel { }
    property QtObject sourceModel: SourceModel { }

    implicitWidth: units.gridUnit * 30
    implicitHeight: units.gridUnit * 20

    ConfigModule.quickHelp: i18n("This module allows to set up the Pulseaudio sound subsystem.")

    Tab {
        title: i18nc("@title:tab", "Applications")
        SinkInputView {
            model: PulseObjectFilterModel {
                filters: [ { role: "VirtualStream", value: false } ]
                sourceModel: SinkInputModel {}
            }
            emptyText: i18nc("@label", "No Applications Playing Audio")
        }
    }
    Tab {
        title: i18nc("@title:tab", "Recording")
        SourceOutputView {
            model: PulseObjectFilterModel {
                filters: [ { role: "VirtualStream", value: false } ]
                sourceModel: SourceOutputModel {}
            }
            emptyText: i18nc("@label", "No Applications Recording Audio")
        }
    }
    Tab {
        title: i18nc("@title:tab", "Output Devices")
        DeviceView {
            model: sinkModel
            emptyText: i18nc("@label", "No Output Devices Available")
        }
    }
    Tab {
        title: i18nc("@title:tab", "Input Devices")
        DeviceView {
            model: sourceModel
            emptyText: i18nc("@label", "No Input Devices Available")
        }
    }
    Tab {
        title: i18nc("@title:tab", "Configuration")
        CardView {
            model: CardModel {}
            emptyText: i18nc("@label", "No Additional Configuration Available")
        }
    }
}
