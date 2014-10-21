#include "sourceoutput.h"

SourceOutput::SourceOutput()
    : m_name()
    , m_sourceIndex(0)
    , m_volume()
    , m_isMuted(false)
{
}

void SourceOutput::setInfo(const pa_source_output_info *info)
{
    m_index = info->index;
    m_name = QString::fromUtf8(info->name);
    m_sourceIndex = info->source;
    m_volume = info->volume;
    m_isMuted = info->mute;
}
