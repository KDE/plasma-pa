import QtQuick 2.0

BaseItem {
    label: Description
    icon: 'audio-card'

    function setVolume(volume) {
        // TODO: possibly set the volume icon here, otherwise it will update on delay
        pulseContext.setSinkVolume(Index, volume);
    }
}
