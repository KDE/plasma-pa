/*
    Copyright 2014-2015 Harald Sitter <sitter@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CARD_H
#define CARD_H

#include <pulse/introspect.h>

#include <QMap>
#include <QVariant>

#include "profile.h"
#include "port.h"
#include "pulseobject.h"

namespace QPulseAudio
{

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
                qCDebug(PLASMAPA) << "property" << key << "not a string";
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

    QString name() const;
    QList<QObject *> profiles() const;
    quint32 activeProfileIndex() const;
    void setActiveProfileIndex(quint32 profileIndex);
    QList<QObject *> ports() const;

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

} // QPulseAudio

#endif // CARD_H
