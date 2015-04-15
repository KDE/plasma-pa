#ifndef ABSTRACTVOLUMEBASE_H
#define ABSTRACTVOLUMEBASE_H

#include <QtGlobal>

#include <pulse/volume.h>

class Q_DECL_EXPORT AbstractVolumeBase
{
public:
    virtual ~AbstractVolumeBase() {}

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

protected:
    pa_cvolume m_volume;
    bool m_muted = false;
};

#endif // ABSTRACTVOLUMEBASE_H
