#ifndef SOURCE_H
#define SOURCE_H

#include "devicebase.h"

class Q_DECL_EXPORT Source : public DeviceBase<pa_source_info, pa_source_port_info>
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(QString description READ description)
    Q_PROPERTY(QList<Port> ports READ ports)
    Q_PROPERTY(int activePortIndex READ activePortIndex)

//    Q_PROPERTY(qint64 volume READ volume WRITE setVolume)
    Q_PROPERTY(bool muted READ isMuted WRITE setMuted)
public:
    void setVolume(qint64 volume) Q_DECL_OVERRIDE;
    void setMuted(bool muted) Q_DECL_OVERRIDE;
};

#endif // SOURCE_H
