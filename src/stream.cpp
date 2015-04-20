#include "stream.h"

StreamBase::StreamBase(QObject *parent)
    : VolumeObject(parent)
{
}

StreamBase::~StreamBase()
{
}

QString StreamBase::name() const
{
    return m_name;
}

bool StreamBase::hasVolume() const
{
    return m_hasVolume;
}

bool StreamBase::isVolumeWritable() const
{
    return m_isVolumeWritable;
}

Client *StreamBase::client() const
{
    qDebug() <<"client";
    return context()->clients().data().value(m_clientIndex, nullptr);
}
