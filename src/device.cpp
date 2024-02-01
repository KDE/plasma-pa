/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "device.h"
#include "device_p.h"

namespace PulseAudioQt
{
Device::State Device::state() const
{
    return d->m_state;
}

QString Device::description() const
{
    return d->m_description;
}

QString Device::formFactor() const
{
    return d->m_formFactor;
}

quint32 Device::cardIndex() const
{
    return d->m_cardIndex;
}

QList<Port *> Device::ports() const
{
    return d->m_ports;
}

quint32 Device::activePortIndex() const
{
    return d->m_activePortIndex;
}

qint64 Device::baseVolume() const
{
    return d->m_baseVolume;
}

QVariantMap Device::pulseProperties() const
{
    return d->m_pulseProperties;
}

bool Device::isVirtualDevice() const
{
    return d->m_virtualDevice;
}

Device::Device(QObject *parent)
    : VolumeObject(parent)
    , d(new DevicePrivate(this))
{
}

DevicePrivate::DevicePrivate(Device *q)
    : q(q)
{
}

Device::State DevicePrivate::stateFromPaState(int value) const
{
    switch (value) {
    case -1: // PA_X_INVALID_STATE
        return Device::InvalidState;
    case 0: // PA_X_RUNNING
        return Device::RunningState;
    case 1: // PA_X_IDLE
        return Device::IdleState;
    case 2: // PA_X_SUSPENDED
        return Device::SuspendedState;
    default:
        return Device::UnknownState;
    }
}

Device::~Device()
{
    delete d;
}

} // namespace PulseAudioQt
