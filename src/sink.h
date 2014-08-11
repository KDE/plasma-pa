#ifndef SINK_H
#define SINK_H

#include <pulse/volume.h>

#include "pulseobject.h"

class Sink : public PulseObject
{
    Q_OBJECT
public:
    void setInfo(const pa_sink_info *info);

    QString name() const { return m_name; }
    QString description() const { return m_description; }
    pa_cvolume volume() const { return m_volume; }

private:
    QString m_name;
    QString m_description;
    pa_cvolume m_volume;
};

#endif // SINK_H
