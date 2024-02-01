/*
    SPDX-FileCopyrightText: 2018 Nicolas Fella <nicolas.fella@gmx.de>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/
#ifndef SOURCE_P_H
#define SOURCE_P_H

#include "source.h"
#include <pulse/introspect.h>

namespace PulseAudioQt
{
class SourcePrivate
{
public:
    explicit SourcePrivate(Source *q);

    void update(const pa_source_info *info);

    Source *q;
};
}
#endif
