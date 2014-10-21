#include "sourceoutput.h"

SourceOutput::SourceOutput()
    : m_name()
    , m_sourceIndex(0)
    , m_volume()
    , m_isMuted(false)
    , m_hasVolume(false)
    , m_isVolumeWritable(false)
{
}

void SourceOutput::setInfo(const pa_source_output_info *info)
{
    m_index = info->index;
    m_name = QString::fromUtf8(info->name);
    m_sourceIndex = info->source;
    m_volume = info->volume;
    m_isMuted = info->mute;
    m_hasVolume = info->has_volume;
    m_isVolumeWritable = info->volume_writable

    m_client = info->client;
}
