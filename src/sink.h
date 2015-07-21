#ifndef SINK_H
#define SINK_H

#include "device.h"

namespace QPulseAudio
{

class Q_DECL_EXPORT Sink : public Device
{
    Q_OBJECT
public:
    Sink(QObject *parent);

    void update(const pa_sink_info *info);
    void setVolume(qint64 volume) Q_DECL_OVERRIDE;
    void setMuted(bool muted) Q_DECL_OVERRIDE;
    void setActivePortIndex(quint32 port_index) Q_DECL_OVERRIDE;
};

} // QPulseAudio

#endif // SINK_H
