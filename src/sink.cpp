#include "sink.h"

#include <QDebug>

SinkPort::SinkPort()
    : m_name()
    , m_description()
    , m_priority(0)
    , m_isAvailable(false)
{
}

void SinkPort::setInfo(const pa_sink_port_info *port)
{
    m_name = port->name;
    m_description = port->description;
    m_priority = port->priority;
    m_isAvailable = port->available;
}

Sink::Sink()
    : m_name()
    , m_description()
    , m_volume()
    , m_isMuted(false)
    , m_ports()
    , m_activePortIndex(-1)
{
}

void Sink::setInfo(const pa_sink_info *info)
{
    m_name = info->name;
    m_index = info->index;
    m_description = info->description;

    m_volume = info->volume;
    m_isMuted = info->mute;

    m_ports.clear();
    for (auto **ports = info->ports; *ports != nullptr; ++ports) {
        SinkPort port;
        port.setInfo(*ports);
        m_ports.append(port);
        if (info->active_port == *ports) {
            m_activePortIndex = m_ports.size() - 1;
        }
    }

    qDebug() << info->name << info->volume.values[0] << info->driver;
}
