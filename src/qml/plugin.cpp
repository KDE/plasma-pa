/*
    Copyright 2014-2015 Harald Sitter <sitter@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "plugin.h"

#include <QQmlEngine>

#include "pulseaudio.h"
#include "client.h"
#include "sink.h"
#include "source.h"
#include "context.h"
#include "modulemanager.h"
#include "profile.h"
#include "port.h"
#include "volumemonitor.h"

#include "globalactioncollection.h"
#include "microphoneindicator.h"
#include "volumeosd.h"
#include "volumefeedback.h"

static QJSValue pulseaudio_singleton(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)

    QJSValue object = scriptEngine->newObject();
    object.setProperty(QStringLiteral("NormalVolume"), (double) QPulseAudio::Context::NormalVolume);
    object.setProperty(QStringLiteral("MinimalVolume"), (double) QPulseAudio::Context::MinimalVolume);
    object.setProperty(QStringLiteral("MaximalVolume"), (double) QPulseAudio::Context::MaximalVolume);
    return object;
}

void Plugin::registerTypes(const char* uri)
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
    qmlRegisterUncreatableType<QPulseAudio::Profile>(uri, 0, 1, "Profile", QString());
    qmlRegisterUncreatableType<QPulseAudio::Port>(uri, 0, 1, "Port", QString());
    qmlRegisterType<GlobalAction>(uri, 0, 1, "GlobalAction");
    qmlRegisterType<GlobalActionCollection>(uri, 0, 1, "GlobalActionCollection");
    qmlRegisterType<VolumeOSD>(uri, 0, 1, "VolumeOSD");
    qmlRegisterType<VolumeFeedback>(uri, 0, 1, "VolumeFeedback");
    qmlRegisterSingletonType(uri, 0, 1, "PulseAudio", pulseaudio_singleton);
    qmlRegisterSingletonType<MicrophoneIndicator>(uri, 0, 1, "MicrophoneIndicator",
        [](QQmlEngine *engine, QJSEngine *jsEngine) -> QObject* {
            Q_UNUSED(engine);
            Q_UNUSED(jsEngine);
            return new MicrophoneIndicator();
    });
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
    qmlRegisterType<QPulseAudio::Client>();
    qmlRegisterType<QPulseAudio::Sink>();
    qmlRegisterType<QPulseAudio::Source>();
    qmlRegisterType<QPulseAudio::VolumeObject>();
#else
    qmlRegisterAnonymousType<QPulseAudio::Client>(uri, 1);
    qmlRegisterAnonymousType<QPulseAudio::Sink>(uri, 1);
    qmlRegisterAnonymousType<QPulseAudio::Source>(uri, 1);
    qmlRegisterAnonymousType<QPulseAudio::VolumeObject>(uri, 1);
#endif
}
