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

#include <QtQml>

#include "pulseaudio.h"
#include "client.h"
#include "sink.h"
#include "source.h"

#include "globalactioncollection.h"
#include "volumeosd.h"

void Plugin::registerTypes(const char* uri)
{
    qmlRegisterType<QPulseAudio::CardModel>(uri, 0, 1, "CardModel");
    qmlRegisterType<QPulseAudio::SinkModel>(uri, 0, 1, "SinkModel");
    qmlRegisterType<QPulseAudio::SinkInputModel>(uri, 0, 1, "SinkInputModel");
    qmlRegisterType<QPulseAudio::SourceModel>(uri, 0, 1, "SourceModel");
    qmlRegisterType<QPulseAudio::SourceOutputModel>(uri, 0, 1, "SourceOutputModel");
    qmlRegisterType<QPulseAudio::StreamRestoreModel>(uri, 0, 1, "StreamRestoreModel");
    qmlRegisterType<GlobalAction>(uri, 0, 1, "GlobalAction");
    qmlRegisterType<GlobalActionCollection>(uri, 0, 1, "GlobalActionCollection");
    qmlRegisterType<VolumeOSD>(uri, 0, 1, "VolumeOSD");

    qmlRegisterType<QPulseAudio::Client>();
    qmlRegisterType<QPulseAudio::Sink>();
    qmlRegisterType<QPulseAudio::Source>();
}
