#include "card.h"

#include <QDebug>

Card::Card(QObject *parent)
    : PulseObject(parent)
{
}

void Card::update(const pa_card_info *info)
{
    updatePulseObject(info);
    m_name = QString::fromUtf8(info->name);
    m_driver = QString::fromUtf8(info->driver);

    qDebug() << "processing card" << info->index << info->name;

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

#warning there is no active port prop so it is possible that this is actually very worthless since we have sinks and sources
    qDeleteAll(m_ports);
    m_ports.clear();
    for (auto **it = info->ports; it && *it != nullptr; ++it) {
        Port *port = new Port(this);
        port->setInfo(*it);
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
