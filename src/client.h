#ifndef CLIENT_H
#define CLIENT_H

#include <pulse/introspect.h>

#include <QMap>

#include "pulseobject.h"

class Q_DECL_EXPORT Client : public PulseObject
{
    Q_OBJECT
public:
    Client();

    void setInfo(const pa_client_info *info);

    QString name() const { return m_name; }
    QMap<QString, QVariant> properties() const { return m_properties; }

private:
    QString m_name;
    QString m_ownerModule;
    QString m_driver;
    QMap<QString, QVariant> m_properties;
};

#endif // CLIENT_H
