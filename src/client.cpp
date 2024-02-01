/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "client.h"

#include "debug.h"
#include "indexedpulseobject_p.h"

namespace PulseAudioQt
{
Client::Client(QObject *parent)
    : IndexedPulseObject(parent)
{
}

Client::~Client() = default;

void Client::update(const pa_client_info *info)
{
    IndexedPulseObject::d->updatePulseObject(info);
    PulseObject::d->updateProperties(info);

    const QString infoName = QString::fromUtf8(info->name);
    if (m_name != infoName) {
        m_name = infoName;
        Q_EMIT nameChanged();
    }
}

QString Client::name() const
{
    return m_name;
}

} // PulseAudioQt
