#include "stream.h"

Stream::Stream(QObject *parent)
    : VolumeObject(parent)
{
}

Stream::~Stream()
{
}

QString Stream::name() const
{
    return m_name;
}

bool Stream::hasVolume() const
{
    return m_hasVolume;
}

bool Stream::isVolumeWritable() const
{
    return m_isVolumeWritable;
}

Client *Stream::client() const
{
    qDebug() <<"client";
    return context()->clients().data().value(m_clientIndex, nullptr);
}
