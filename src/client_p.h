/*
    SPDX-FileCopyrightText: 2018 Nicolas Fella <nicolas.fella@gmx.de>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/
#ifndef CLIENT_P_H
#define CLIENT_P_H

#include "client.h"
#include <pulse/introspect.h>

namespace PulseAudioQt
{
class ClientPrivate
{
public:
    ClientPrivate(Client *q);

    void update(const pa_client_info *info);

    Client *q;
};

} // PulseAudioQt

#endif
