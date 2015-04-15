#ifndef SINK_H
#define SINK_H

#include "devicebase.h"

class Q_DECL_EXPORT Sink : public DeviceBase<pa_sink_info, pa_sink_port_info>
{
    Q_OBJECT
public:
    void setVolume(qint64 volume) Q_DECL_OVERRIDE;
    void setMuted(bool muted) Q_DECL_OVERRIDE;
};

#endif // SINK_H
