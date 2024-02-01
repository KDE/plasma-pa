/*
    SPDX-FileCopyrightText: 2018 Nicolas Fella <nicolas.fella@gmx.de>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/
#ifndef DEVICE_P_H
#define DEVICE_P_H

#include <pulse/proplist.h>

#include <QHash>
#include <QVector>

#include "device.h"
#include "port.h"
#include "port_p.h"
#include "volumeobject_p.h"

namespace PulseAudioQt
{
class DevicePrivate
{
public:
    explicit DevicePrivate(Device *q);

    Device *q;

    QString m_description;
    QString m_formFactor;
    quint32 m_cardIndex = -1;
    QList<Port *> m_ports;
    quint32 m_activePortIndex = -1;
    Device::State m_state = Device::UnknownState;
    qint64 m_baseVolume = -1;
    QVariantMap m_pulseProperties;
    bool m_virtualDevice = false;

    Device::State stateFromPaState(int value) const;

    template<typename PAInfo>
    void updatePulseProperties(const PAInfo *info)
    {
        QVariantMap pulseProperties;
        void *state = nullptr;
        while (auto key = pa_proplist_iterate(info->proplist, &state)) {
            const auto value = pa_proplist_gets(info->proplist, key);
            pulseProperties.insert(key, QString::fromUtf8(value));
        }
        if (pulseProperties != m_pulseProperties) {
            m_pulseProperties = pulseProperties;
            Q_EMIT q->pulsePropertiesChanged();
        }
    }

    template<typename PAInfo>
    void updateDevice(const PAInfo *info)
    {
        q->VolumeObject::d->updateVolumeObject(info);

        if (m_description != info->description) {
            m_description = info->description;
            Q_EMIT q->descriptionChanged();
        }
        const char *form_factor = pa_proplist_gets(info->proplist, PA_PROP_DEVICE_FORM_FACTOR);
        if (form_factor) {
            QString formFactor = QString::fromUtf8(form_factor);
            if (m_formFactor != formFactor) {
                m_formFactor = formFactor;
                Q_EMIT q->formFactorChanged();
            }
        }

        m_cardIndex = info->card;
        Q_EMIT q->cardIndexChanged();

        updatePulseProperties(info);

        QStringList newPorts;
        QStringList existingPorts;

        // Build list of existing ports
        for (const Port *port : qAsConst(m_ports)) {
            existingPorts << port->name();
        }

        // Add new ports from the updated port list and re/set port info
        for (auto **it = info->ports; it && *it != nullptr; ++it) {
            const QString name = QString::fromUtf8((*it)->name);
            newPorts << name;

            Port *port = nullptr;

            if (existingPorts.contains(name)) {
                port = m_ports[existingPorts.indexOf(name)];
            } else {
                port = new Port(q);
                m_ports << port;
            }

            port->d->setInfo(*it);
        }

        // Remove ports that are not in the updated port list
        for (Port *port : qAsConst(m_ports)) {
            if (!newPorts.contains(port->name())) {
                m_ports.removeOne(port);
                delete port;
            }
        }

        // Set active port
        for (Port *port : qAsConst(m_ports)) {
            if (info->active_port->name == port->name()) {
                m_activePortIndex = m_ports.indexOf(port);
            }
        }

        Q_EMIT q->portsChanged();
        Q_EMIT q->activePortIndexChanged();

        Device::State infoState = stateFromPaState(info->state);
        if (infoState != m_state) {
            m_state = infoState;
            Q_EMIT q->stateChanged();
        }

        if (m_baseVolume != info->base_volume) {
            m_baseVolume = info->base_volume;
            Q_EMIT q->baseVolumeChanged();
        }

        const bool isVirtual = !(info->flags & 4); // PA_X_HARDWARE
        if (m_virtualDevice != isVirtual) {
            m_virtualDevice = isVirtual;
            Q_EMIT q->virtualDeviceChanged();
        }
    }
};

} // namespace PulseAudioQt

#endif
