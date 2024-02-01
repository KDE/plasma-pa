/*
    SPDX-FileCopyrightText: 2018 Nicolas Fella <nicolas.fella@gmx.de>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/
#ifndef SINKINPUT_P_H
#define SINKINPUT_P_H

#include "sinkinput.h"
#include <pulse/introspect.h>

namespace PulseAudioQt
{
class SinkInputPrivate
{
public:
    explicit SinkInputPrivate(SinkInput *q);

    SinkInput *q;

    void update(const pa_sink_input_info *info);
};
}

#endif
