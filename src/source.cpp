#include "source.h"

Source::Source()
    : m_name()
    , m_description()
    , m_volume()
    , m_isMuted(false)
{
}

void Source::setInfo(const pa_source_info *info)
{
    m_index = info->index;
    m_name = QString::fromUtf8(info->name);
    m_description = QString::fromUtf8(info->description);
    m_volume = info->volume;
    m_isMuted = info->mute;
}
