/*
    SPDX-FileCopyrightText: 2018 Nicolas Fella <nicolas.fella@gmx.de>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/
#pragma once
#include "server.h"
#include <pulse/introspect.h>

namespace PulseAudioQt
{
class ServerPrivate
{
public:
    explicit ServerPrivate(Server *q);
    virtual ~ServerPrivate();

    Server *q;

    QString m_defaultSinkName;
    QString m_defaultSourceName;
    Sink *m_defaultSink;
    Source *m_defaultSource;
    bool m_isPipeWire = false;

    void update(const pa_server_info *info);
};
}
