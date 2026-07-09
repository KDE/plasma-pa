/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef PLUGIN_H
#define PLUGIN_H

#include <PulseAudioQt/Client>
#include <PulseAudioQt/Context>
#include <PulseAudioQt/Models>
#include <PulseAudioQt/Server>
#include <PulseAudioQt/Sink>
#include <PulseAudioQt/SinkInput>
#include <PulseAudioQt/SourceOutput>
#include <PulseAudioQt/StreamRestore>
#include <PulseAudioQt/VolumeObject>

#include <globalconfig.h>

#include <QQmlEngine>
#include <QQmlExtensionPlugin>
#include <qqmlregistration.h>

class QJSEngine;

class CardModelForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(CardModel)
    QML_FOREIGN(PulseAudioQt::CardModel)
};

class SinkModelForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(SinkModel)
    QML_FOREIGN(PulseAudioQt::SinkModel)
};

class SinkInputModelForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(SinkInputModel)
    QML_FOREIGN(PulseAudioQt::SinkInputModel)
};

class SinkInputForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(SinkInput)
    QML_UNCREATABLE("")
    QML_FOREIGN(PulseAudioQt::SinkInput)
};

class SourceModelForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(SourceModel)
    QML_FOREIGN(PulseAudioQt::SourceModel)
};

class SourceOutputModelForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(SourceOutputModel)
    QML_FOREIGN(PulseAudioQt::SourceOutputModel)
};

class SourceOutputForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(SourceOutput)
    QML_UNCREATABLE("")
    QML_FOREIGN(PulseAudioQt::SourceOutput)
};

class StreamRestoreModelForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(StreamRestoreModel)
    QML_FOREIGN(PulseAudioQt::StreamRestoreModel)
};

class StreamRestoreForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(StreamRestore)
    QML_UNCREATABLE("")
    QML_FOREIGN(PulseAudioQt::StreamRestore)
};

class ModuleModelForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(ModuleModel)
    QML_FOREIGN(PulseAudioQt::ModuleModel)
};

class PulseObjectForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(PulseObject)
    QML_UNCREATABLE("")
    QML_FOREIGN(PulseAudioQt::PulseObject)
};

class ProfileForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(Profile)
    QML_UNCREATABLE("")
    QML_FOREIGN(PulseAudioQt::Profile)
};

class PortForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(Port)
    QML_UNCREATABLE("")
    QML_FOREIGN(PulseAudioQt::Port)
};

class VolumeObjectForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(VolumeObject)
    QML_UNCREATABLE("")
    QML_FOREIGN(PulseAudioQt::VolumeObject)
};

class SourceForeign
{
    Q_GADGET
    QML_ANONYMOUS
    QML_FOREIGN(PulseAudioQt::Source)
};

class SinkForeign
{
    Q_GADGET
    QML_ANONYMOUS
    QML_FOREIGN(PulseAudioQt::Sink)
};

class ClientForeign
{
    Q_GADGET
    QML_ANONYMOUS
    QML_FOREIGN(PulseAudioQt::Client)
};

class GlobalConfigForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(GlobalConfig)
    QML_FOREIGN(GlobalConfig)
};

class ServerForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(Server)
    QML_SINGLETON
    QML_FOREIGN(PulseAudioQt::Server)

public:
    static PulseAudioQt::Server *create(QQmlEngine *, QJSEngine *)
    {
        auto server = PulseAudioQt::Context::instance()->server();
        QQmlEngine::setObjectOwnership(server, QQmlEngine::CppOwnership);
        return server;
    };
};

class ContextForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(Context)
    QML_SINGLETON
    QML_FOREIGN(PulseAudioQt::Context)

public:
    static PulseAudioQt::Context *create(QQmlEngine *, QJSEngine *)
    {
        // Created on-call rather than on-registration to not start the Context "too early".
        auto context = PulseAudioQt::Context::instance();
        QQmlEngine::setObjectOwnership(context, QQmlEngine::CppOwnership);
        return context;
    };
};

class Plugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")
public:
    void registerTypes(const char *uri) override;
};

#endif // PLUGIN_H
