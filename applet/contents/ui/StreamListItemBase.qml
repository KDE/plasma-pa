import QtQuick 2.0

import org.kde.plasma.components 2.0 as PlasmaComponents

import org.kde.plasma.private.volume 0.1

ListItemBase {
    label: PulseObject.client.name + ": " + PulseObject.name
    icon: {
        if (PulseObject.client.properties['application.icon_name']) {
            return PulseObject.client.properties['application.icon_name'].toLowerCase();
        } else if (PulseObject.client.properties['application.process.binary']) {
            if (PulseObject.client.properties['application.process.binary'].toLowerCase() === 'chrome') {
                return 'google-chrome';
            }
            return PulseObject.client.properties['application.process.binary'].toLowerCase();
        }
        return 'unknown';
    }
}
