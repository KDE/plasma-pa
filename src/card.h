#ifndef CARD_H
#define CARD_H

#include <pulse/introspect.h>

#include <QMap>
#include <QVariant>

#include "pulseobject.h"

template <typename PAInfo>
class ProfileBase
{
public:
    ProfileBase()
        : m_name()
        , m_description()
        , m_priority(0)
    {
    }

    virtual ~ProfileBase() {}

    virtual void setInfo(const PAInfo *info)
    {
        // Description is optional. Name not so much as we need some ID.
        Q_ASSERT(info->name);
        m_name = QString::fromUtf8(info->name);
        if (info->description) {
            m_description = QString::fromUtf8(info->description);
        }
        m_priority = info->priority;
    }

    QString name() const { return m_name; }
    QString description() const { return m_description; }
    quint32 priority() const { return m_priority; }

private:
    QString m_name;
    QString m_description;
    quint32 m_priority;
};

template <typename PAInfo>
class PortBase : public ProfileBase<PAInfo>
{
public:
    PortBase()
        : ProfileBase<PAInfo>()
        , m_isAvailable(false)
    {
    }

    virtual ~PortBase() {}

    virtual void setInfo(const PAInfo *info) Q_DECL_OVERRIDE
    {
        ProfileBase<PAInfo>::setInfo(info);
        m_isAvailable = info->available;
    }

    bool isAvailable() const { return m_isAvailable; }

private:
    bool m_isAvailable;
};

typedef ProfileBase<pa_card_profile_info> CardProfile;
typedef PortBase<pa_card_port_info> CardPort;

class Card : public PulseObject
{
    Q_OBJECT
public:
    Card();

    void setInfo(const pa_card_info *info);

    QString name() const { return m_name; }
    QString driver() const { return m_driver; }

    QList<CardProfile> profiles() const { return m_profiles; }
    int activeProfileIndex() const { return m_activeProfileIndex; }

    QList<CardPort> ports() const { return m_ports; }

    QMap<QString, QVariant> properties() const { return m_properties; }

private:
    QString m_name;
    QString m_ownerModule;
    QString m_driver;

    QList<CardProfile> m_profiles;
    int m_activeProfileIndex;

    QList<CardPort> m_ports;

    QMap<QString, QVariant> m_properties;
};

#endif // CARD_H
