#include "volumeobject.h"

VolumeObject::VolumeObject(QObject *parent)
    : PulseObject(parent)
{
}

VolumeObject::~VolumeObject()
{
}

#warning volume value is a bit meh with channels in the picture
qint64 VolumeObject::volume() const
{
    return m_volume.values[0];
}

bool VolumeObject::isMuted() const
{
    return m_muted;
}

pa_cvolume VolumeObject::cvolume() const
{
    return m_volume;
}
