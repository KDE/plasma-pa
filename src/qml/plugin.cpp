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

#include <PulseAudioQt/Models>
#include <PulseAudioQt/Client>
#include <PulseAudioQt/Sink>
#include <PulseAudioQt/Source>
#include <PulseAudioQt/Context>
#include <PulseAudioQt/Profile>
#include <PulseAudioQt/Port>

#include "modulemanager.h"
#include "globalactioncollection.h"
#include "microphoneindicator.h"
#include "volumeosd.h"
#include "volumefeedback.h"

static QJSValue pulseaudio_singleton(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)

    QJSValue object = scriptEngine->newObject();
    object.setProperty(QStringLiteral("NormalVolume"), (double) PulseAudioQt::normalVolume());
    object.setProperty(QStringLiteral("MinimalVolume"), (double) PulseAudioQt::minimumVolume());
    object.setProperty(QStringLiteral("MaximalVolume"), (double) PulseAudioQt::maximumVolume());
    return object;
}

void Plugin::registerTypes(const char* uri)
{
    qmlRegisterType<PulseAudioQt::CardModel>(uri, 0, 1, "CardModel");
    qmlRegisterType<PulseAudioQt::SinkModel>(uri, 0, 1, "SinkModel");
    qmlRegisterType<PulseAudioQt::SinkInputModel>(uri, 0, 1, "SinkInputModel");
    qmlRegisterType<PulseAudioQt::SourceModel>(uri, 0, 1, "SourceModel");
    qmlRegisterType<PulseAudioQt::SourceOutputModel>(uri, 0, 1, "SourceOutputModel");
    qmlRegisterType<PulseAudioQt::StreamRestoreModel>(uri, 0, 1, "StreamRestoreModel");
    qmlRegisterType<PulseAudioQt::ModuleModel>(uri, 0, 1, "ModuleModel");
    qmlRegisterUncreatableType<PulseAudioQt::Profile>(uri, 0, 1, "Profile", QString());
    qmlRegisterUncreatableType<PulseAudioQt::Port>(uri, 0, 1, "Port", QString());
    qmlRegisterType<GlobalAction>(uri, 0, 1, "GlobalAction");
    qmlRegisterType<GlobalActionCollection>(uri, 0, 1, "GlobalActionCollection");
    qmlRegisterType<VolumeOSD>(uri, 0, 1, "VolumeOSD");
    qmlRegisterType<VolumeFeedback>(uri, 0, 1, "VolumeFeedback");
    qmlRegisterType<ModuleManager>(uri, 0, 1, "ModuleManager");
    qmlRegisterSingletonType(uri, 0, 1, "PulseAudio", pulseaudio_singleton);
    qmlRegisterSingletonType<MicrophoneIndicator>(uri, 0, 1, "MicrophoneIndicator",
        [](QQmlEngine *engine, QJSEngine *jsEngine) -> QObject* {
            Q_UNUSED(engine);
            Q_UNUSED(jsEngine);
            return new MicrophoneIndicator();
    });
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
    qmlRegisterType<PulseAudioQt::Client>();
    qmlRegisterType<PulseAudioQt::Sink>();
    qmlRegisterType<PulseAudioQt::Source>();
#else
    qmlRegisterAnonymousType<PulseAudioQt::Client>(uri, 1);
    qmlRegisterAnonymousType<PulseAudioQt::Sink>(uri, 1);
    qmlRegisterAnonymousType<PulseAudioQt::Source>(uri, 1);
#endif
}
