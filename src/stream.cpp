#include "stream.h"

Stream::Stream(QObject *parent)
    : VolumeObject(parent)
{
    m_volumeWritable = false;
    m_hasVolume = false;
}

Stream::~Stream()
{
}

QString Stream::name() const
{
    return m_name;
}

Client *Stream::client() const
{
    return context()->clients().data().value(m_clientIndex, nullptr);
}
