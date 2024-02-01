/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "client.h"
#include "client_p.h"

#include "debug.h"
#include "indexedpulseobject_p.h"

namespace PulseAudioQt
{
Client::Client(QObject *parent)
    : IndexedPulseObject(parent)
    , d(new ClientPrivate(this))
{
}

ClientPrivate::ClientPrivate(Client *q)
    : q(q)
{
}

Client::~Client()
{
    delete d;
}

void ClientPrivate::update(const pa_client_info *info)
{
    q->IndexedPulseObject::d->updatePulseObject(info);
    q->PulseObject::d->updateProperties(info);
}

} // PulseAudioQt
