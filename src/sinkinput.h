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
    bool hasVolume() const { return m_hasVolume; }
    bool isVolumeWritable() const { return m_isVolumeWritable; }

private:
    QString m_name;
    pa_cvolume m_volume;
    bool m_hasVolume;
    bool m_isVolumeWritable;
};

#endif // SINKINPUT_H
