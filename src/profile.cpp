/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "profile.h"
#include "profile_p.h"

namespace PulseAudioQt
{
Profile::Profile(QObject *parent)
    : PulseObject(parent)
    , d(new ProfilePrivate(this))
{
}

Profile::~Profile()
{
    delete d;
}

ProfilePrivate::ProfilePrivate(Profile *q)
    : q(q)
{
}

ProfilePrivate::~ProfilePrivate()
{
}

QString Profile::description() const
{
    return d->m_description;
}

quint32 Profile::priority() const
{
    return d->m_priority;
}

Profile::Availability Profile::availability() const
{
    return d->m_availability;
}

quint32 Profile::sinkCount() const
{
    return d->m_sinkCount;
}

quint32 Profile::sourceCount() const
{
    return d->m_sourceCount;
}

} // PulseAudioQt
