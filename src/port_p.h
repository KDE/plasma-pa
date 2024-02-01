/*
    SPDX-FileCopyrightText: 2018 Nicolas Fella <nicolas.fella@gmx.de>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#pragma once

#include "port.h"
#include "profile_p.h"
#include <pulse/def.h>

namespace PulseAudioQt
{
class PortPrivate
{
public:
    explicit PortPrivate(Port *q);
    virtual ~PortPrivate();

    Port *q;
    Port::Type m_type = Port::Type::Unknown;

    template<typename PAInfo>
    void setInfo(const PAInfo *info)
    {
        Profile::Availability newAvailability;
        switch (info->available) {
        case PA_PORT_AVAILABLE_NO:
            newAvailability = Profile::Unavailable;
            break;
        case PA_PORT_AVAILABLE_YES:
            newAvailability = Profile::Available;
            break;
        default:
            newAvailability = Profile::Unknown;
        }

#if PA_CHECK_VERSION(14, 0, 0)
        m_type = static_cast<Port::Type>(info->type);
#endif
        Q_EMIT q->typeChanged();

        q->Profile::d->setCommonInfo(info, newAvailability);
    }
};
}
