#ifndef SINKINPUT_H
#define SINKINPUT_H

#include "stream.h"

namespace QPulseAudio
{

class Q_DECL_EXPORT SinkInput : public Stream
{
    Q_OBJECT
    Q_PROPERTY(quint32 sinkIndex READ sinkIndex WRITE setSinkIndex NOTIFY sinkIndexChanged)
public:
    SinkInput(QObject *parent);

    void update(const pa_sink_input_info *info);

    quint32 sinkIndex() const;
    void setSinkIndex(quint32 sinkIndex);

    void setVolume(qint64 volume) Q_DECL_OVERRIDE;
    void setMuted(bool muted) Q_DECL_OVERRIDE;

signals:
    void sinkIndexChanged();

private:
    quint32 m_sinkIndex = 0;
};

} // QPulseAudio

#endif // SINKINPUT_H
