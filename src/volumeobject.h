#ifndef VOLUMEOBJECT_H
#define VOLUMEOBJECT_H

#include <pulse/volume.h>

#include "pulseobject.h"

namespace QPulseAudio
{

class VolumeObject : public PulseObject
{
    Q_OBJECT
    Q_PROPERTY(qint64 volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(bool muted READ isMuted WRITE setMuted NOTIFY mutedChanged)
    Q_PROPERTY(bool hasVolume READ hasVolume NOTIFY hasVolumeChanged)
    Q_PROPERTY(bool volumeWritable READ isVolumeWritable NOTIFY isVolumeWritableChanged)
public:
    VolumeObject(QObject *parent);
    virtual ~VolumeObject();

    template <typename PAInfo>
    void updateVolumeObject(PAInfo *info)
    {
        updatePulseObject(info);
        if (m_muted != info->mute) {
            m_muted = info->mute;
            emit mutedChanged();
        }
#warning fixme volume needs manual compare
//            if (m_volume != info->volume) {
                m_volume = info->volume;
                emit volumeChanged();
//            }
    }

    qint64 volume() const;
    virtual void setVolume(qint64 volume) = 0;

    bool isMuted() const;
    virtual void setMuted(bool muted) = 0;

    bool hasVolume() const;
    bool isVolumeWritable() const;

signals:
    void volumeChanged();
    void mutedChanged();
    void hasVolumeChanged();
    void isVolumeWritableChanged();

protected:
    pa_cvolume cvolume() const;

    pa_cvolume m_volume;
    bool m_muted;
    bool m_hasVolume;
    bool m_volumeWritable;
};

} // QPulseAudio

#endif // VOLUMEOBJECT_H
