/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

const highUpperBound = 100;
const veryHighUpperBound = 125;

function name(volume, muted, prefix) {
    if (!prefix) {
        prefix = "audio-volume";
    }
    var icon = null;
    const percent = main.volumePercent(volume);
    if (percent <= 0 || muted) {
        icon = prefix + "-muted";
    } else if (percent <= 25) {
        icon = prefix + "-low";
    } else if (percent <= 75) {
        icon = prefix + "-medium";
    } else if (percent <= highUpperBound) {
        icon = prefix + "-high";
    } else if (percent <= veryHighUpperBound) {
        icon = `${prefix}-high-warning`;
    } else {
        icon = `${prefix}-high-danger`;
    }
    return icon;
}

function formFactorIcon(formFactor) {
    switch(formFactor) {
        case "internal":
            return "audio-card";
        case "speaker":
            return "audio-speakers-symbolic";
        case "phone":
            return "phone";
        case "handset":
            return "phone";
        case "tv":
            return "video-television";
        case "webcam":
            return "camera-web";
        case "microphone":
            return "audio-input-microphone";
        case "headset":
            return "audio-headset";
        case "headphone":
            return "audio-headphones";
        case "hands-free":
            return "hands-free";
        case "car":
            return "car";
        case "hifi":
            return "hifi";
        case "computer":
            return "computer";
        case "portable":
            return "portable";
    }
    return "";
}
