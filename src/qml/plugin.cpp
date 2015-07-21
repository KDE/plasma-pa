#include "plugin.h"

#include <QtQml>

#include "pulseaudio.h"
#include "client.h"

#include "globalactioncollection.h"
#include "volumeosd.h"

void Plugin::registerTypes(const char* uri)
{
    qmlRegisterType<Context>();
    qmlRegisterType<CardModel>(uri, 0, 1, "CardModel");
    qmlRegisterType<ClientModel>(uri, 0, 1, "ClientModel");
    qmlRegisterType<SinkModel>(uri, 0, 1, "SinkModel");
    qmlRegisterType<SinkInputModel>(uri, 0, 1, "SinkInputModel");
    qmlRegisterType<ReverseFilterModel>(uri, 0, 1, "ReverseFilterModel");
    qmlRegisterType<SourceModel>(uri, 0, 1, "SourceModel");
    qmlRegisterType<SourceOutputModel>(uri, 0, 1, "SourceOutputModel");
    qmlRegisterType<GlobalAction>(uri, 0, 1, "GlobalAction");
    qmlRegisterType<GlobalActionCollection>(uri, 0, 1, "GlobalActionCollection");
    qmlRegisterType<VolumeOSD>(uri, 0, 1, "VolumeOSD");
    qmlRegisterType<Client>();
}
