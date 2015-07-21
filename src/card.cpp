#include "card.h"

#include <QDebug>

#include "context.h"

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
#warning more inconsistent api... profiels end with entry with name on null...
    for (auto *it = info->profiles; it->name != nullptr; ++it) {
        Profile *profile = new Profile(this);
        profile->setInfo(it);
        m_profiles.append(profile);
        if (info->active_profile == it) {
            m_activeProfileIndex = m_profiles.length() - 1;
        }
    }

    emit profilesChanged();
    emit activeProfileIndexChanged();

    }
}

void Card::setActiveProfileIndex(quint32 profileIndex)
{
    const Profile *profile = qobject_cast<Profile *>(profiles().at(profileIndex));
    context()->setCardProfile(index(), profile->name());
}
