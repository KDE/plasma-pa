#include "sinkinput.h"

#include <QDebug>

SinkInput::SinkInput(QObject *parent)
    : PulseObject(parent)
    , m_name()
    , m_volume()
    , m_hasVolume(false)
    , m_isVolumeWritable(false)
{
}

void SinkInput::setInfo(const pa_sink_input_info *info)
{
    m_index = info->index;
    m_name = QString::fromUtf8(info->name);
    m_hasVolume = info->has_volume;
    m_isVolumeWritable = info->volume_writable;
    if (m_hasVolume) {
        m_volume = info->volume;
    }
    qDebug() << Q_FUNC_INFO << info->name << info->index;
}
