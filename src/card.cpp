#include "card.h"

#include <QDebug>

Card::Card()
{
}

void Card::setInfo(const pa_card_info *info)
{
    m_index = info->index;
    m_name = QString::fromUtf8(info->name);
    m_driver = QString::fromUtf8(info->driver);

    qDebug() << "processing card" << info->index << info->name;

    m_profiles.clear();
#warning more inconsistent api... profiels end with entry with name on null...
    for (auto *it = info->profiles; it->name != nullptr; ++it) {
        Profile profile;
        profile.setInfo(it);
        m_profiles.append(profile);
        if (info->active_profile == it) {
            qDebug() << "+++++++++++++++++ profile " << it->description << " appears the active one at index: "<< m_profiles.length() - 1;
            m_activeProfileIndex = m_profiles.length() - 1;
        }
    }

#warning there is no active port prop so it is possible that this is actually very worthless since we have sinks and sources
    m_ports.clear();
    for (auto **it = info->ports; it && *it != nullptr; ++it) {
        Port port;
        port.setInfo(*it);
        m_ports.append(port);
    }

    void *it = nullptr;
    while (const char *key = pa_proplist_iterate(info->proplist, &it)) {
        Q_ASSERT(key);
        const char *value = pa_proplist_gets(info->proplist, key);
        if (!value) {
            qDebug() << "property" << key << "not a string";
            continue;
        }
        Q_ASSERT(value);
        m_properties.insert(QString::fromUtf8(key), QString::fromUtf8(value));
    }
}
