#ifndef SINK_H
#define SINK_H

#include <pulse/volume.h>

#include "portbase.h"
#include "pulseobject.h"

class Q_DECL_EXPORT Sink : public PulseObject
{
    Q_OBJECT
public:
    typedef PortBase<pa_sink_port_info> Port;

    Sink();

    void setInfo(const pa_sink_info *info);

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
    bool m_isMuted;
    QList<Port> m_ports;
    int m_activePortIndex;
};

#endif // SINK_H
