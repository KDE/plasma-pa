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
    bool isAvailable() const { return m_isAvailable; }

private:
    QString m_name;
    QString m_description;
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
    QList<SinkPort> ports() const { return m_ports; }
    bool activePortIndex() const { return m_activePortIndex; }

private:
    QString m_name;
    QString m_description;
    pa_cvolume m_volume;
    QList<SinkPort> m_ports;
    int m_activePortIndex;
};

#endif // SINK_H
