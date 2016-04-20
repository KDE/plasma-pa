/*
    Copyright 2016 David Rosca <nowrep@gmail.com>

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

#include "server.h"
#include "context.h"
#include "sink.h"
#include "source.h"
#include "debug.h"

namespace QPulseAudio
{

Server::Server(QObject *parent)
    : QObject(parent)
    , m_defaultSink(nullptr)
    , m_defaultSource(nullptr)
{
}

Sink *Server::defaultSink() const
{
    return m_defaultSink;
}

void Server::setDefaultSink(Sink *sink)
{
    Q_ASSERT(sink);
    Context::instance()->setDefaultSink(sink->name());
}

Source *Server::defaultSource() const
{
    return m_defaultSource;
}

void Server::setDefaultSource(Source *source)
{
    Q_ASSERT(source);
    Context::instance()->setDefaultSource(source->name());
}

void Server::reset()
{
    if (m_defaultSink) {
        m_defaultSink = nullptr;
        emit defaultSinkChanged(m_defaultSink);
    }

    if (m_defaultSource) {
        m_defaultSource = nullptr;
        emit defaultSourceChanged(m_defaultSource);
    }
}

template <typename Type, typename Map>
static Type *findByName(const Map &map, const char *name)
{
    const QString nameStr = QString::fromUtf8(name);
    Type *out = nullptr;
    QMapIterator<quint32, Type *> it(map);
    while (it.hasNext()) {
        it.next();
        out = it.value();
        if (out->name() == nameStr) {
            return out;
        }
    }
    qCWarning(PLASMAPA) << "No object for name" << nameStr;
    return out;
}

void Server::update(const pa_server_info *info)
{
    Sink *sink = findByName<Sink>(Context::instance()->sinks().data(), info->default_sink_name);
    Source *source = findByName<Source>(Context::instance()->sources().data(), info->default_source_name);

    if (m_defaultSink != sink) {
        m_defaultSink = sink;
        emit defaultSinkChanged(m_defaultSink);
    }

    if (m_defaultSource != source) {
        m_defaultSource = source;
        emit defaultSourceChanged(m_defaultSource);
    }
}

} // QPulseAudio
