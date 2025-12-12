// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>
// SPDX-FileCopyrightText: 2016 David Rosca <nowrep@gmail.com>

#include "preferreddevice.h"

#include <PulseAudioQt/Context>
#include <PulseAudioQt/Server>
#include <PulseAudioQt/Sink>
#include <PulseAudioQt/Source>

PreferredDevice::PreferredDevice(QObject *parent)
    : QObject(parent)
{
    connect(PulseAudioQt::Context::instance()->server(), &PulseAudioQt::Server::defaultSinkChanged, this, &PreferredDevice::updatePreferredSink);
    connect(PulseAudioQt::Context::instance()->server(), &PulseAudioQt::Server::defaultSourceChanged, this, &PreferredDevice::updatePreferredSource);
}

PulseAudioQt::Sink *PreferredDevice::sink() const
{
    return m_sink;
}

PulseAudioQt::Source *PreferredDevice::source() const
{
    return m_source;
}

void PreferredDevice::updatePreferredSink()
{
    auto sink = PulseAudioQt::Context::instance()->server()->defaultSink();
    if (sink != m_sink) {
        m_sink = sink;
        Q_EMIT sinkChanged();
    }
}

void PreferredDevice::updatePreferredSource()
{
    auto source = PulseAudioQt::Context::instance()->server()->defaultSource();
    if (source != m_source) {
        m_source = source;
        Q_EMIT sourceChanged();
    }
}

#include "moc_preferreddevice.cpp"
