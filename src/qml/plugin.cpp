/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "plugin.h"

#include <QQmlEngine>

#include "client.h"
#include "context.h"
#include "modulemanager.h"
#include "port.h"
#include "profile.h"
#include "pulseaudio.h"
#include "sink.h"
#include "source.h"
#include "volumemonitor.h"

#include "globalactioncollection.h"
#include "listitemmenu.h"
#include "microphoneindicator.h"
#include "volumefeedback.h"
#include "volumeosd.h"

static QJSValue pulseaudio_singleton(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)

    QJSValue object = scriptEngine->newObject();
    object.setProperty(QStringLiteral("NormalVolume"), (double)QPulseAudio::Context::NormalVolume);
    object.setProperty(QStringLiteral("MinimalVolume"), (double)QPulseAudio::Context::MinimalVolume);
    object.setProperty(QStringLiteral("MaximalVolume"), (double)QPulseAudio::Context::MaximalVolume);
    return object;
}

void Plugin::registerTypes(const char *uri)
{
    qmlRegisterType<QPulseAudio::CardModel>(uri, 0, 1, "CardModel");
    qmlRegisterType<QPulseAudio::SinkModel>(uri, 0, 1, "SinkModel");
    qmlRegisterType<QPulseAudio::SinkInputModel>(uri, 0, 1, "SinkInputModel");
    qmlRegisterType<QPulseAudio::SourceModel>(uri, 0, 1, "SourceModel");
    qmlRegisterType<QPulseAudio::ModuleManager>(uri, 0, 1, "ModuleManager");
    qmlRegisterType<QPulseAudio::SourceOutputModel>(uri, 0, 1, "SourceOutputModel");
    qmlRegisterType<QPulseAudio::StreamRestoreModel>(uri, 0, 1, "StreamRestoreModel");
    qmlRegisterType<QPulseAudio::ModuleModel>(uri, 0, 1, "ModuleModel");
    qmlRegisterType<QPulseAudio::VolumeMonitor>(uri, 0, 01, "VolumeMonitor");
    qmlRegisterUncreatableType<QPulseAudio::PulseObject>(uri, 0, 1, "PulseObject", QString());
    qmlRegisterUncreatableType<QPulseAudio::Profile>(uri, 0, 1, "Profile", QString());
    qmlRegisterUncreatableType<QPulseAudio::Port>(uri, 0, 1, "Port", QString());
    qmlRegisterType<GlobalAction>(uri, 0, 1, "GlobalAction");
    qmlRegisterType<GlobalActionCollection>(uri, 0, 1, "GlobalActionCollection");
    qmlRegisterType<ListItemMenu>(uri, 0, 1, "ListItemMenu");
    qmlRegisterType<VolumeOSD>(uri, 0, 1, "VolumeOSD");
    qmlRegisterType<VolumeFeedback>(uri, 0, 1, "VolumeFeedback");
    qmlRegisterSingletonType(uri, 0, 1, "PulseAudio", pulseaudio_singleton);
    qmlRegisterSingletonType<MicrophoneIndicator>(uri, 0, 1, "MicrophoneIndicator", [](QQmlEngine *engine, QJSEngine *jsEngine) -> QObject * {
        Q_UNUSED(engine);
        Q_UNUSED(jsEngine);
        return new MicrophoneIndicator();
    });
    qmlRegisterAnonymousType<QPulseAudio::Client>(uri, 1);
    qmlRegisterAnonymousType<QPulseAudio::Sink>(uri, 1);
    qmlRegisterAnonymousType<QPulseAudio::Source>(uri, 1);
    qmlRegisterAnonymousType<QPulseAudio::VolumeObject>(uri, 1);
}
