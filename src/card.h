#ifndef CARD_H
#define CARD_H

#include <pulse/introspect.h>

#include <QMap>
#include <QVariant>

#include "portbase.h"
#include "pulseobject.h"

class Q_DECL_EXPORT Card : public PulseObject
{
    Q_OBJECT
public:
    typedef ProfileBase<pa_card_profile_info> Profile;
    typedef PortBase<pa_card_port_info> Port;

    Card();

    void setInfo(const pa_card_info *info);

    QString name() const { return m_name; }
    QString driver() const { return m_driver; }

    QList<Profile> profiles() const { return m_profiles; }
    int activeProfileIndex() const { return m_activeProfileIndex; }

    QList<Port> ports() const { return m_ports; }

    QMap<QString, QVariant> properties() const { return m_properties; }

private:
    QString m_name;
    QString m_ownerModule;
    QString m_driver;

    QList<Profile> m_profiles;
    int m_activeProfileIndex;

    QList<Port> m_ports;

    QMap<QString, QVariant> m_properties;
};

#endif // CARD_H
