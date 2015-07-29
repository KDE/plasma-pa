import QtQuick 2.1
import QtQuick.Controls 1.3

import org.kde.kcm 1.0
import org.kde.plasma.core 2.0 as PlasmaCore

import org.kde.plasma.private.volume 0.1

TabView {
    implicitWidth: units.gridUnit * 20
    implicitHeight: units.gridUnit * 20

    ConfigModule.quickHelp: "((UNKNOWN))"
    Tab {
        title: i18nc("@title:tab", "Output Devices")
        DeviceView {
            model: SinkModel {}
        }
    }
    Tab {
        title: i18nc("@title:tab", "Input Devices")
        DeviceView {
            model: SourceModel {}
        }
    }
    Tab {
        title: i18nc("@title:tab", "Applications")
        StreamView {
            model: SinkInputModel {}
        }
    }
    Tab {
        title: i18nc("@title:tab", "Recording")
        StreamView {
            model: SourceOutputModel {}
        }
    }
    Tab {
        title: i18nc("@title:tab", "Configuration")
        CardView {
            model: CardModel {}
        }
    }
}
