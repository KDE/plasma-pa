/*
    SPDX-FileCopyrightText: 2023 Bharadwaj Raju <bharadwaj.raju777@protonmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "audioicon.h"

#include <QMap>
#include <QString>

using namespace Qt::Literals::StringLiterals;

const QMap<QString, QString> formFactorMap = {
    {u"internal"_s, u"audio-card"_s},
    {u"speaker"_s, u"audio-speakers-symbolic"_s},
    {u"phone"_s, u"phone"_s},
    {u"handset"_s, u"phone"_s},
    {u"tv"_s, u"video-television"_s},
    {u"webcam"_s, u"camera-web"_s},
    {u"microphone"_s, u"audio-input-microphone"_s},
    {u"headset"_s, u"audio-headset"_s},
    {u"headphone"_s, u"audio-headphones"_s},
    {u"hands-free"_s, u"hands-free"_s},
    {u"car"_s, u"car"_s},
    {u"hifi"_s, u"hifi"_s},
    {u"computer"_s, u"computer"_s},
    {u"portable"_s, u"portable"_s},
};

QString AudioIcon::forFormFactor(QString formFactor)
{
    return formFactorMap.value(formFactor, "");
}

QString AudioIcon::forVolume(int percent, bool muted, QString prefix)
{
    if (prefix.isEmpty()) {
        prefix = u"audio-volume"_s;
    }
    if (percent <= 0 || muted) {
        return prefix + "-muted";
    } else if (percent <= 25) {
        return prefix + "-low";
    } else if (percent <= 75) {
        return prefix + "-medium";
    } else if (percent <= HIGH_UPPER_BOUND) {
        return prefix + "-high";
    } else if (percent <= VERY_HIGH_UPPER_BOUND) {
        return prefix + "-high-warning";
    } else {
        return prefix + "-high-danger";
    }
}
