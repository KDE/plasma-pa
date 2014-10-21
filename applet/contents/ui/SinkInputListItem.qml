import QtQuick 2.0

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

    function setVolume(volume) {
        pulseContext.setSinkInputVolume(Index, volume);
    }
}
