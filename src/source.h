#ifndef SOURCE_H
#define SOURCE_H

#include "devicebase.h"

class Q_DECL_EXPORT Source : public DeviceBase<pa_source_info, pa_source_port_info>
{
    Q_OBJECT
public:
    void setVolume(qint64 volume) Q_DECL_OVERRIDE;
    void setMuted(bool muted) Q_DECL_OVERRIDE;
};

#endif // SOURCE_H
