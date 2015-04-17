#ifndef ABSTRACTVOLUMEBASE_H
#define ABSTRACTVOLUMEBASE_H

#include <QtGlobal>

#include <pulse/volume.h>

#include "pulseobject.h"

class VolumeObject : public PulseObject
{
    Q_OBJECT
    Q_PROPERTY(qint64 volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(bool muted READ isMuted WRITE setMuted NOTIFY mutedChanged)
public:
    virtual ~VolumeObject() {}

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

#warning cvolume should be protected and only used internally
    Q_DECL_DEPRECATED pa_cvolume cvolume() const
    {
        return m_volume;
    }
#warning volume value is a bit meh with channels in the picture
    qint64 volume() const
    {
        return m_volume.values[0];
    }
    virtual void setVolume(qint64 volume) = 0;

    bool isMuted() const
    {
        return m_muted;
    }
    virtual void setMuted(bool muted) = 0;

signals:
    void volumeChanged();
    void mutedChanged();

protected:
    pa_cvolume m_volume;
    bool m_muted = false;
};


#endif // ABSTRACTVOLUMEBASE_H
