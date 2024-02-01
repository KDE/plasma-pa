/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "stream.h"
#include "context_p.h"
#include "stream_p.h"
#include "volumeobject_p.h"

namespace PulseAudioQt
{
Stream::Stream(QObject *parent)
    : VolumeObject(parent)
    , d(new StreamPrivate(this))
{
    VolumeObject::d->m_volumeWritable = false;
}

Stream::~Stream()
{
    delete d;
}

StreamPrivate::StreamPrivate(Stream *q)
    : q(q)
{
}

StreamPrivate::~StreamPrivate()
{
}

Client *Stream::client() const
{
    return Context::instance()->d->m_clients.data().value(d->m_clientIndex, nullptr);
}

bool Stream::isVirtualStream() const
{
    return d->m_virtualStream;
}

quint32 Stream::deviceIndex() const
{
    return d->m_deviceIndex;
}

bool Stream::isCorked() const
{
    return d->m_corked;
}

bool Stream::hasVolume() const
{
    return d->m_hasVolume;
}

} // PulseAudioQt
