/*
    SPDX-FileCopyrightText: 2016 David Rosca <nowrep@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "server.h"
#include "server_p.h"

#include "context.h"
#include "context_p.h"
#include "debug.h"
#include "sink.h"
#include "source.h"

namespace PulseAudioQt
{
Server::Server(Context *context)
    : QObject(context)
    , d(new ServerPrivate(this))
{
    Q_ASSERT(context);

    connect(&context->d->m_sinks, &MapBaseQObject::added, this, &Server::updateDefaultDevices);
    connect(&context->d->m_sinks, &MapBaseQObject::removed, this, &Server::updateDefaultDevices);
    connect(&context->d->m_sources, &MapBaseQObject::added, this, &Server::updateDefaultDevices);
    connect(&context->d->m_sources, &MapBaseQObject::removed, this, &Server::updateDefaultDevices);
}

Server::~Server()
{
    delete d;
}

ServerPrivate::ServerPrivate(Server *q)
    : q(q)
    , m_defaultSink(nullptr)
    , m_defaultSource(nullptr)
{
}

ServerPrivate::~ServerPrivate()
{
}

Sink *Server::defaultSink() const
{
    return d->m_defaultSink;
}

void Server::setDefaultSink(Sink *sink)
{
    Q_ASSERT(sink);
    Context::instance()->setDefaultSink(sink->name());
}

Source *Server::defaultSource() const
{
    return d->m_defaultSource;
}

void Server::setDefaultSource(Source *source)
{
    Q_ASSERT(source);
    Context::instance()->setDefaultSource(source->name());
}

void Server::reset()
{
    if (d->m_defaultSink) {
        d->m_defaultSink = nullptr;
        Q_EMIT defaultSinkChanged(d->m_defaultSink);
    }

    if (d->m_defaultSource) {
        d->m_defaultSource = nullptr;
        Q_EMIT defaultSourceChanged(d->m_defaultSource);
    }
}

void ServerPrivate::update(const pa_server_info *info)
{
    m_defaultSinkName = QString::fromUtf8(info->default_sink_name);
    m_defaultSourceName = QString::fromUtf8(info->default_source_name);

    const bool isPw = QString::fromUtf8(info->server_name).contains("PipeWire");

    if (isPw != m_isPipeWire) {
        m_isPipeWire = isPw;
        Q_EMIT q->isPipeWireChanged();
    }

    q->updateDefaultDevices();

    Q_EMIT q->updated();
}

/** @private */
template<typename Type, typename Vector>
static Type *findByName(const Vector &vector, const QString &name)
{
    Type *out = nullptr;
    if (name.isEmpty()) {
        return out;
    }
    for (Type *t : vector) {
        out = t;
        if (out->name() == name) {
            return out;
        }
    }
    qCWarning(PULSEAUDIOQT) << "No object for name" << name;
    return out;
}

void Server::updateDefaultDevices()
{
    Sink *sink = findByName<Sink>(Context::instance()->d->m_sinks.data(), d->m_defaultSinkName);
    Source *source = findByName<Source>(Context::instance()->d->m_sources.data(), d->m_defaultSourceName);

    if (d->m_defaultSink != sink) {
        qCDebug(PULSEAUDIOQT) << "Default sink changed" << sink;
        d->m_defaultSink = sink;
        Q_EMIT defaultSinkChanged(d->m_defaultSink);
    }

    if (d->m_defaultSource != source) {
        qCDebug(PULSEAUDIOQT) << "Default source changed" << source;
        d->m_defaultSource = source;
        Q_EMIT defaultSourceChanged(d->m_defaultSource);
    }
}

bool Server::isPipeWire() const
{
    return d->m_isPipeWire;
}

} // PulseAudioQt
