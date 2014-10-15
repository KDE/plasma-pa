#ifndef SINKINPUT_H
#define SINKINPUT_H

#include "pulseobject.h"

class SinkInput : public PulseObject
{
    Q_OBJECT
public:
    SinkInput(QObject *parent = nullptr);

    void setInfo(const pa_sink_input_info *info);

    QString name() const { return m_name; }
    pa_cvolume volume() const { return m_volume; }
    bool isMuted() const { return m_isMuted; }
    bool hasVolume() const { return m_hasVolume; }
    bool isVolumeWritable() const { return m_isVolumeWritable; }

    quint32 client() const { return m_client; }

private:
    QString m_name;
    pa_cvolume m_volume;
    bool m_isMuted;
    bool m_hasVolume;
    bool m_isVolumeWritable;

    quint32 m_client;
};

#endif // SINKINPUT_H
