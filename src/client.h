#ifndef CLIENT_H
#define CLIENT_H

#include <pulse/introspect.h>

#include "pulseobject.h"

class Client : public PulseObject
{
    Q_OBJECT
public:
    Client();

    void setInfo(const pa_client_info *info);

    QString name() const { return m_name; }

private:
    QString m_name;
    QString m_ownerModule;
    QString m_driver;
};

#endif // CLIENT_H
