#ifndef SINK_H
#define SINK_H

#include "devicebase.h"

class Q_DECL_EXPORT Sink : public DeviceBase
{
    Q_OBJECT
public:
    void update(const pa_sink_info *info);
    void setVolume(qint64 volume) Q_DECL_OVERRIDE;
    void setMuted(bool muted) Q_DECL_OVERRIDE;
    void setActivePortIndex(quint32 port_index) Q_DECL_OVERRIDE;
};

#endif // SINK_H
