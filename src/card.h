#ifndef CARD_H
#define CARD_H

#include <pulse/introspect.h>

#include <QMap>
#include <QVariant>

#include "portbase.h"
#include "pulseobject.h"

typedef ProfileBase<pa_card_profile_info> CardProfile;
typedef PortBase<pa_card_port_info> CardPort;

class Q_DECL_EXPORT Card : public PulseObject
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
