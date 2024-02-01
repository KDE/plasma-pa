/*
    SPDX-FileCopyrightText: 2018 Nicolas Fella <nicolas.fella@gmx.de>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/
#ifndef SINK_P_H
#define SINK_P_H

#include "pulse/introspect.h"
#include "sink.h"

namespace PulseAudioQt
{
class SinkPrivate
{
public:
    explicit SinkPrivate(Sink *q);

    void update(const pa_sink_info *info);

    Sink *q;
    quint32 m_monitorIndex = -1;
};
}
#endif
