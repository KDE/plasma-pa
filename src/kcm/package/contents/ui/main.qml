import QtQuick 2.1
import QtQuick.Controls 1.3

import org.kde.kcm 1.0
import org.kde.kquickcontrolsaddons 2.0
import org.kde.plasma.core 2.0 as PlasmaCore

TabView {
    implicitWidth: units.gridUnit * 20
    implicitHeight: units.gridUnit * 20

    ConfigModule.quickHelp: "((UNKNOWN))"

    Tab {
        title: i18nc("@title:tab", "Applications")
        SinkInputs {}
    }
    Tab {
        title: i18nc("@title:tab", "Recording")
    }
    Tab {
        title: i18nc("@title:tab", "Output Devices")
        Sinks {}
    }
    Tab {
        title: i18nc("@title:tab", "Input Devices")
    }
    Tab {
        title: i18nc("@title:tab", "Configuration")
        Cards {}
    }
}
