#include "sinkinput.h"

#include <QDebug>

SinkInput::SinkInput(QObject *parent)
    : PulseObject(parent)
    , m_name()
    , m_volume()
    , m_isMuted(false)
    , m_hasVolume(false)
    , m_isVolumeWritable(false)
    , m_client(0)
{
}

void SinkInput::setInfo(const pa_sink_input_info *info)
{
#warning todo channelmap what does it do do we need it and meow
    m_index = info->index;
    m_name = QString::fromUtf8(info->name);
    m_isMuted = info->mute;
    m_hasVolume = info->has_volume;
    m_isVolumeWritable = info->volume_writable;
    if (m_hasVolume) {
        m_volume = info->volume;
    }
    m_client = info->client;
    qDebug() << Q_FUNC_INFO << info->name << info->index;
}
