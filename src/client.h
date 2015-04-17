#ifndef CLIENT_H
#define CLIENT_H

#include <pulse/introspect.h>

#include <QMap>

#include "pulseobject.h"

class Q_DECL_EXPORT Client : public PulseObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QVariantMap properties READ properties NOTIFY propertiesChanged)
public:
    Client();

    void update(const pa_client_info *info);

    QString name() const { return m_name; }
    QVariantMap properties() const { return m_properties; }

signals:
    void nameChanged();
    void propertiesChanged();

private:
    QString m_name;
    QVariantMap m_properties;
};

#endif // CLIENT_H
