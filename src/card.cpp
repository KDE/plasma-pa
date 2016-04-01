/*
    Copyright 2014-2015 Harald Sitter <sitter@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "card.h"

#include "debug.h"

#include "context.h"

namespace QPulseAudio
{

Card::Card(QObject *parent)
    : PulseObject(parent)
{
}

void Card::update(const pa_card_info *info)
{
    updatePulseObject(info);

    QString infoName = QString::fromUtf8(info->name);
    if (m_name != infoName) {
        m_name = infoName;
        emit nameChanged();
    }

    qDeleteAll(m_profiles);
    m_profiles.clear();
    for (auto **it = info->profiles2; it && *it != nullptr; ++it) {
        Profile *profile = new Profile(this);
        profile->setInfo(*it);
        m_profiles.append(profile);
        if (info->active_profile2 == *it) {
            m_activeProfileIndex = m_profiles.length() - 1;
        }
    }
    emit profilesChanged();
    emit activeProfileIndexChanged();

    qDeleteAll(m_ports);
    m_ports.clear();
    for (auto **it = info->ports; it && *it != nullptr; ++it) {
        CardPort *port = new CardPort(this);
        port->update(*it);
        m_ports.append(port);
    }
    emit portsChanged();
}

QString Card::name() const
{
    return m_name;
}

QList<QObject *> Card::profiles() const
{
    return m_profiles;
}

quint32 Card::activeProfileIndex() const
{
    return m_activeProfileIndex;
}

void Card::setActiveProfileIndex(quint32 profileIndex)
{
    const Profile *profile = qobject_cast<Profile *>(profiles().at(profileIndex));
    context()->setCardProfile(index(), profile->name());
}

QList<QObject *> Card::ports() const
{
    return m_ports;
}

} // QPulseAudio
