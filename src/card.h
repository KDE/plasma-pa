#ifndef CARD_H
#define CARD_H

#include <pulse/introspect.h>

#include <QMap>
#include <QVariant>

#include "profile.h"
#include "port.h"
#include "pulseobject.h"

class CardPort : public Port
{
    Q_OBJECT
    Q_PROPERTY(QVariantMap properties READ properties NOTIFY propertiesChanged)
public:
    CardPort(QObject *parent = nullptr) : Port(parent) {}
    virtual ~CardPort() {}

//    int direction;                      /**< A #pa_direction enum, indicating the direction of this port. */
//    uint32_t n_profiles;                /**< Number of entries in profile array */
//    pa_card_profile_info** profiles;    /**< \deprecated Superseded by profiles2 */
//    int64_t latency_offset;             /**< Latency offset of the port that gets added to the sink/source latency when the port is active. \since 3.0 */
//    pa_card_profile_info2** profiles2;  /**< Array of pointers to available profiles, or NULL. Array is terminated by an entry set to NULL. \since 5.0 */

    void update(const pa_card_port_info *info)
    {
        setInfo(info);

        m_properties.clear();
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
        emit propertiesChanged();
    }

    QVariantMap properties() const { return m_properties; }

signals:
    void propertiesChanged();

private:
    QVariantMap m_properties;
};

class Q_DECL_EXPORT Card : public PulseObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QList<QObject *> profiles READ profiles  NOTIFY profilesChanged)
    Q_PROPERTY(quint32 activeProfileIndex READ activeProfileIndex WRITE setActiveProfileIndex NOTIFY activeProfileIndexChanged)
    Q_PROPERTY(QList<QObject *> ports READ ports NOTIFY portsChanged)
public:
    Card(QObject *parent);

    void update(const pa_card_info *info);

    QString name() const { return m_name; }
    QList<QObject *> profiles() const { return m_profiles; }
    quint32 activeProfileIndex() const { return m_activeProfileIndex; }
    void setActiveProfileIndex(quint32 profileIndex);
    QList<QObject *> ports() const { return m_ports; }

signals:
    void nameChanged();
    void profilesChanged();
    void activeProfileIndexChanged();
    void portsChanged();

private:
    QString m_name;
    QList<QObject *> m_profiles;
    quint32 m_activeProfileIndex;
    QList<QObject *> m_ports;
};

#endif // CARD_H
