#ifndef ABSTRACTVOLUMEBASE_H
#define ABSTRACTVOLUMEBASE_H

#include <QtGlobal>

#include <pulse/volume.h>

class Q_DECL_EXPORT AbstractVolumeBase
{
public:
    virtual ~AbstractVolumeBase() {}

    pa_cvolume volume() const
    {
        return m_volume;
    }
    virtual void setVolume(qint64 volume) = 0;

    bool isMuted() const
    {
        return m_muted;
    }
    virtual void setMuted(bool muted) = 0;

protected:
    pa_cvolume m_volume;
    bool m_muted = false;
};

#endif // ABSTRACTVOLUMEBASE_H
