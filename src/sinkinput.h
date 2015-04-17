#ifndef SINKINPUT_H
#define SINKINPUT_H

#include "streambase.h"

class Q_DECL_EXPORT SinkInput : public StreamBase
{
    Q_OBJECT
#warning missing index property
public:
    void update(const pa_sink_input_info *info);

    quint32 sinkIndex() const { return m_sinkIndex; }

    void setVolume(qint64 volume) Q_DECL_OVERRIDE;
    void setMuted(bool muted) Q_DECL_OVERRIDE;

private:
    quint32 m_sinkIndex = 0;
};

#endif // SINKINPUT_H
