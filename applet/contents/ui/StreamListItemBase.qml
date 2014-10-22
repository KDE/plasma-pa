import QtQuick 2.0

import org.kde.plasma.components 2.0 as PlasmaComponents

import org.kde.plasma.volume 0.1

ListItemBase {
    label: ClientName + ": " + Name
    icon: {
        if (ClientProperties['application.icon_name']) {
            return ClientProperties['application.icon_name'].toLowerCase();
        } else if (ClientProperties['application.process.binary']) {
            if (ClientProperties['application.process.binary'].toLowerCase() === 'chrome') {
                return 'google-chrome';
            }
            return ClientProperties['application.process.binary'].toLowerCase();
        }
        return 'unknown';
    }
}
