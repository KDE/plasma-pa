#ifndef STREAMBASE_H
#define STREAMBASE_H

#include <QString>

#include <pulse/volume.h>

#include "pulseobject.h"

template <typename PAInfo>
class Q_DECL_EXPORT StreamBase : public PulseObject
{
public:
    StreamBase() {}
    virtual ~StreamBase() {}

    virtual void setInfo(const PAInfo *info)
    {
        m_index = info->index;
        m_name = QString::fromUtf8(info->name);
        m_isMuted = info->mute;
        m_hasVolume = info->has_volume;
        m_isVolumeWritable = info->volume_writable;
        if (m_hasVolume) {
            m_volume = info->volume;
        }
        m_client = info->client;
    }

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

#endif // STREAMBASE_H
