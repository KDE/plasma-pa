/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "device.h"

PulseAudioQt::Device::State PulseAudioQt::Device::state() const
{
    return m_state;
}

QString PulseAudioQt::Device::name() const
{
    return m_name;
}

QString PulseAudioQt::Device::description() const
{
    return m_description;
}

QString PulseAudioQt::Device::formFactor() const
{
    return m_formFactor;
}

quint32 PulseAudioQt::Device::cardIndex() const
{
    return m_cardIndex;
}

QList<QObject *> PulseAudioQt::Device::ports() const
{
    return m_ports;
}

quint32 PulseAudioQt::Device::activePortIndex() const
{
    return m_activePortIndex;
}

bool PulseAudioQt::Device::isVirtualDevice() const
{
    return m_virtualDevice;
}

QVariantMap PulseAudioQt::Device::pulseProperties() const
{
    return m_pulseProperties;
}

PulseAudioQt::Device::Device(QObject *parent)
    : VolumeObject(parent)
{
}

PulseAudioQt::Device::State PulseAudioQt::Device::stateFromPaState(int value) const
{
    switch (value) {
    case -1: // PA_X_INVALID_STATE
        return InvalidState;
    case 0: // PA_X_RUNNING
        return RunningState;
    case 1: // PA_X_IDLE
        return IdleState;
    case 2: // PA_X_SUSPENDED
        return SuspendedState;
    default:
        return UnknownState;
    }
}
