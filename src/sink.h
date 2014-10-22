#ifndef SINK_H
#define SINK_H

#include <pulse/volume.h>

#include "pulseobject.h"

class SinkPort
{
public:
    SinkPort();

    void setInfo(const pa_sink_port_info *port);

    QString name() const { return m_name; }
    QString description() const { return m_description; }
    quint32 priority() const { return m_priority; }
    bool isAvailable() const { return m_isAvailable; }

private:
    QString m_name;
    QString m_description;
    quint32 m_priority;
    bool m_isAvailable;
};

class Sink : public PulseObject
{
    Q_OBJECT
public:
    Sink();

    void setInfo(const pa_sink_info *info);

    QString name() const { return m_name; }
    QString description() const { return m_description; }
    pa_cvolume volume() const { return m_volume; }
    bool isMuted() const { return m_isMuted; }
    QList<SinkPort> ports() const { return m_ports; }
    bool activePortIndex() const { return m_activePortIndex; }

private:
    QString m_name;
    QString m_description;
    pa_cvolume m_volume;
    bool m_isMuted;
    QList<SinkPort> m_ports;
    int m_activePortIndex;
};

#endif // SINK_H
