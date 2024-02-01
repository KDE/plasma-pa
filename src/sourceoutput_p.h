/*
    SPDX-FileCopyrightText: 2018 Nicolas Fella <nicolas.fella@gmx.de>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/
#ifndef SOURCEOUTPUT_P_H
#define SOURCEOUTPUT_P_H

#include "sourceoutput.h"
#include <pulse/introspect.h>

namespace PulseAudioQt
{
class SourceOutputPrivate
{
public:
    explicit SourceOutputPrivate(SourceOutput *q);

    void update(const pa_source_output_info *info);

    SourceOutput *q;
};

}

#endif
