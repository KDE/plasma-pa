/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "card.h"
#include "card_p.h"
#include "debug.h"

#include "context.h"
#include "indexedpulseobject_p.h"
#include "port_p.h"
#include "profile_p.h"

namespace PulseAudioQt
{
Card::Card(QObject *parent)
    : IndexedPulseObject(parent)
    , d(new CardPrivate(this))
{
    connect(Context::instance(), &Context::sinkAdded, this, &Card::sinksChanged);
    connect(Context::instance(), &Context::sinkRemoved, this, &Card::sinksChanged);

    connect(Context::instance(), &Context::sourceAdded, this, &Card::sourcesChanged);
    connect(Context::instance(), &Context::sourceRemoved, this, &Card::sourcesChanged);
}

Card::~Card()
{
    delete d;
}

CardPrivate::CardPrivate(Card *q)
    : q(q)
{
}

CardPrivate::~CardPrivate()
{
}

void CardPrivate::update(const pa_card_info *info)
{
    q->IndexedPulseObject::d->updatePulseObject(info);
    q->PulseObject::d->updateProperties(info);

    QStringList newProfiles;
    QStringList existingProfiles;

    for (const Profile *profile : qAsConst(m_profiles)) {
        existingProfiles << profile->name();
    }

    for (auto **it = info->profiles2; it && *it != nullptr; ++it) {
        const QString name = QString::fromUtf8((*it)->name);
        newProfiles << name;
        Profile *profile = nullptr;
        if (existingProfiles.contains(name)) {
            profile = m_profiles[existingProfiles.indexOf(name)];
        } else {
            profile = new Profile(q);
            m_profiles << profile;
        }
        profile->d->setInfo(*it);
    }

    QMutableListIterator<Profile *> it(m_profiles);
    while (it.hasNext()) {
        Profile *profile = it.next();

        if (!newProfiles.contains(profile->name())) {
            it.remove();
            delete profile;
        }
    }

    for (Profile *profile : qAsConst(m_profiles)) {
        if (info->active_profile2->name == profile->name()) {
            m_activeProfileIndex = m_profiles.indexOf(profile);
        }
    }

    Q_EMIT q->profilesChanged();
    Q_EMIT q->activeProfileIndexChanged();

    QStringList newPorts;
    QStringList existingPorts;

    for (const Port *port : qAsConst(m_ports)) {
        existingPorts << port->name();
    }
    for (auto **it = info->ports; it && *it != nullptr; ++it) {
        const QString name = QString::fromUtf8((*it)->name);
        newPorts << name;
        CardPort *port = nullptr;
        if (existingPorts.contains(name)) {
            port = m_ports[existingPorts.indexOf(name)];
        } else {
            port = new CardPort(q);
            m_ports << port;
        }
        port->d->setInfo(*it);
    }

    for (CardPort *port : qAsConst(m_ports)) {
        if (!newPorts.contains(port->name())) {
            m_ports.removeOne(port);
            delete port;
        }
    }

    Q_EMIT q->portsChanged();
}

QList<Profile *> Card::profiles() const
{
    return d->m_profiles;
}

quint32 Card::activeProfileIndex() const
{
    return d->m_activeProfileIndex;
}

void Card::setActiveProfileIndex(quint32 profileIndex)
{
    const Profile *profile = qobject_cast<Profile *>(profiles().at(profileIndex));
    Context::instance()->setCardProfile(index(), profile->name());
}

QList<CardPort *> Card::ports() const
{
    return d->m_ports;
}

QList<Sink *> Card::sinks() const
{
    QList<Sink *> ret;

    const auto allSinks = Context::instance()->sinks();
    for (Sink *sink : allSinks) {
        if (sink->cardIndex() == IndexedPulseObject::d->m_index) {
            ret << sink;
        }
    }

    return ret;
}

QList<Source *> Card::sources() const
{
    QList<Source *> ret;

    const auto allSources = Context::instance()->sources();
    for (Source *source : allSources) {
        if (source->cardIndex() == IndexedPulseObject::d->m_index) {
            ret << source;
        }
    }

    return ret;
}

} // PulseAudioQt
