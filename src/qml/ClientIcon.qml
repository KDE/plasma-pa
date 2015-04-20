import QtQuick 2.0

import org.kde.kquickcontrolsaddons 2.0

QIconItem {
    property QtObject client

    icon: {
        if (client.properties['application.icon_name']) {
            return client.properties['application.icon_name'].toLowerCase();
        } else if (client.properties['application.process.binary']) {
            var binary = client.properties['application.process.binary'].toLowerCase()
            // FIXME: I think this should do a reverse-desktop-file lookup
            // or maybe appdata could be used?
            // At any rate we need to attempt mapping binary to desktop file
            // such that we could get the icon.
            if (binary === 'chrome' || binary === 'chromium') {
                return 'google-chrome';
            }
            return binary;
        }
        return 'unknown';
    }
}
