#ifndef DEVICEBASE_H
#define DEVICEBASE_H

#include <QString>

#include <pulse/volume.h>

#include "portbase.h"
#include "pulseobject.h"

template <typename PAInfo, typename PAPortInfo>
class Q_DECL_EXPORT DeviceBase : public PulseObject
{
public:
    typedef PortBase<PAPortInfo> Port;

    DeviceBase() {}
    virtual ~DeviceBase() {}

    void setInfo(const PAInfo *info)
    {
        m_index = info->index;

        m_name = info->name;
        m_description = info->description;

        m_volume = info->volume;
        m_isMuted = info->mute;

        m_ports.clear();
        for (auto **ports = info->ports; ports && *ports != nullptr; ++ports) {
            Port port;
            port.setInfo(*ports);
            m_ports.append(port);
            if (info->active_port == *ports) {
                m_activePortIndex = m_ports.length() - 1;
            }
        }
    }

    QString name() const { return m_name; }
    QString description() const { return m_description; }
    pa_cvolume volume() const { return m_volume; }
    bool isMuted() const { return m_isMuted; }
    QList<Port> ports() const { return m_ports; }
    int activePortIndex() const { return m_activePortIndex; }

private:
    QString m_name;
    QString m_description;
    pa_cvolume m_volume;
    bool m_isMuted = false;
    QList<Port> m_ports;
    int m_activePortIndex = -1;
};

#endif // DEVICEBASE_H
