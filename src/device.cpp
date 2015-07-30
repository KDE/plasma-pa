#include "device.h"

QString QPulseAudio::Device::name() const
{
    return m_name;
}

QString QPulseAudio::Device::description() const
{
    return m_description;
}

quint32 QPulseAudio::Device::cardIndex() const
{
    return m_cardIndex;
}

QList<QObject *> QPulseAudio::Device::ports() const
{
    return m_ports;
}

quint32 QPulseAudio::Device::activePortIndex() const
{
    return m_activePortIndex;
}

QPulseAudio::Device::Device(QObject *parent)
    : VolumeObject(parent)
{
}
