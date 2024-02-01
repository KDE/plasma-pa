/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "port.h"
#include "port_p.h"
namespace PulseAudioQt
{
Port::Port(QObject *parent)
    : Profile(parent)
    , d(new PortPrivate(this))
{
}

Port::~Port()
{
    delete d;
}

PortPrivate::PortPrivate(Port *q)
    : q(q)
{
}

PortPrivate::~PortPrivate()
{
}

Port::Type Port::type() const
{
    return d->m_type;
}

} // PulseAudioQt
