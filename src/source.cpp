#include "source.h"

Source::Source()
    : m_name()
    , m_description()
    , m_volume()
    , m_isMuted(false)
    , m_ports()
    , m_activePortIndex(-1)
{
}

void Source::setInfo(const pa_source_info *info)
{
    m_index = info->index;
    m_name = QString::fromUtf8(info->name);
    m_description = QString::fromUtf8(info->description);
    m_volume = info->volume;
    m_isMuted = info->mute;

    m_ports.clear();
    for (auto **it = info->ports; it && *it != nullptr; ++it) {
        Port port;
        port.setInfo(*it);
        m_ports.append(port);
    }

    QString activePort = QString::fromUtf8(info->active_port->name);
    for (int i = 0; i < m_ports.length(); ++i) {
        if (m_ports.at(i).name() == activePort) {
            m_activePortIndex = i;
            break;
        }
    }
}
