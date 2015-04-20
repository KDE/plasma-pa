#ifndef VOLUMEOBJECT_H
#define VOLUMEOBJECT_H

#include <pulse/volume.h>

#include "pulseobject.h"

class VolumeObject : public PulseObject
{
    Q_OBJECT
    Q_PROPERTY(qint64 volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(bool muted READ isMuted WRITE setMuted NOTIFY mutedChanged)
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

signals:
    void volumeChanged();
    void mutedChanged();

protected:
    pa_cvolume cvolume() const;

    pa_cvolume m_volume;
    bool m_muted = false;
};


#endif // VOLUMEOBJECT_H
