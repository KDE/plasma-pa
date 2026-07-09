/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>
    SPDX-FileCopyrightText: 2022 ivan (@ratijas) tkachenko <me@ratijas.tk>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "plugin.h"

static QJSValue pulseaudio_singleton(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)

    QJSValue object = scriptEngine->newObject();
    object.setProperty(QStringLiteral("NormalVolume"), (double)PulseAudioQt::normalVolume());
    object.setProperty(QStringLiteral("MinimalVolume"), (double)PulseAudioQt::minimumVolume());
    // The maximum UI value suggested by PulseAudio (PA_VOLUME_UI_MAX) is 153% (11dB)
    // Set the maximum volume to 150% instead
    object.setProperty(QStringLiteral("MaximalVolume"), (double)PulseAudioQt::normalVolume() * 1.5);
    return object;
}

void Plugin::registerTypes(const char *uri)
{
    PulseAudioQt::Context::setApplicationId(QStringLiteral("org.kde.plasma-pa"));

    qmlRegisterSingletonType(uri, 0, 1, "PulseAudio", pulseaudio_singleton);
}

#include "moc_plugin.cpp"
