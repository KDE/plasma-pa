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
#warning notify
    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(QString driver READ driver)
    Q_PROPERTY(QList<QObject *> profiles READ profiles)
    Q_PROPERTY(quint32 activeProfileIndex READ activeProfileIndex)
    Q_PROPERTY(QList<QObject *> ports READ ports)
    Q_PROPERTY(QVariantMap properties READ properties)
public:
    Card();

    void update(const pa_card_info *info);

    QString name() const { return m_name; }
    QString driver() const { return m_driver; }

    QList<QObject *> profiles() const { return m_profiles; }
    quint32 activeProfileIndex() const { return m_activeProfileIndex; }

    QList<QObject *> ports() const { return m_ports; }

    QMap<QString, QVariant> properties() const { return m_properties; }

#warning implement setting profile

private:
    QString m_name;
    QString m_ownerModule;
    QString m_driver;

    QList<QObject *> m_profiles;
    quint32 m_activeProfileIndex;

    QList<QObject *> m_ports;

    QVariantMap m_properties;
};

#endif // CARD_H
