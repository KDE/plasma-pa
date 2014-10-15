#include "plugin.h"

#include <QtQml>

#include "pulseaudio.h"

void Plugin::registerTypes(const char* uri)
{
    qmlRegisterType<Context>(uri, 0, 1, "Context");
    qmlRegisterType<ClientModel>(uri, 0, 1, "ClientModel");
    qmlRegisterType<SinkModel>(uri, 0, 1, "SinkModel");
    qmlRegisterType<SinkInputModel>(uri, 0, 1, "SinkInputModel");
    qmlRegisterType<ReverseSinkInputModel>(uri, 0, 1, "ReverseSinkInputModel");
}
