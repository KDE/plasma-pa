#include "plugin.h"

#include <QtQml>

#include "pulseaudio.h"
#include "client.h"

#include "globalactioncollection.h"
#include "volumeosd.h"

void Plugin::registerTypes(const char* uri)
{
    qmlRegisterType<QPulseAudio::Context>();
    qmlRegisterType<QPulseAudio::CardModel>(uri, 0, 1, "CardModel");
    qmlRegisterType<QPulseAudio::ClientModel>(uri, 0, 1, "ClientModel");
    qmlRegisterType<QPulseAudio::SinkModel>(uri, 0, 1, "SinkModel");
    qmlRegisterType<QPulseAudio::SinkInputModel>(uri, 0, 1, "SinkInputModel");
    qmlRegisterType<QPulseAudio::ReverseFilterModel>(uri, 0, 1, "ReverseFilterModel");
    qmlRegisterType<QPulseAudio::SourceModel>(uri, 0, 1, "SourceModel");
    qmlRegisterType<QPulseAudio::SourceOutputModel>(uri, 0, 1, "SourceOutputModel");
    qmlRegisterType<GlobalAction>(uri, 0, 1, "GlobalAction");
    qmlRegisterType<GlobalActionCollection>(uri, 0, 1, "GlobalActionCollection");
    qmlRegisterType<VolumeOSD>(uri, 0, 1, "VolumeOSD");
    qmlRegisterType<QPulseAudio::Client>();
}
