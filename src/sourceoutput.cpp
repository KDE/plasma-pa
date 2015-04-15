#include "sourceoutput.h"

#include "context.h"

void SourceOutput::setInfo(const pa_source_output_info *info)
{
    StreamBase<pa_source_output_info>::setInfo(info);
    m_sourceIndex = info->source;
}

void SourceOutput::setVolume(qint64 volume)
{
    context()->setGenericVolume(index(), volume, cvolume(), &pa_context_set_source_output_volume);
}

void SourceOutput::setMuted(bool muted)
{
    context()->setGenericMute(index(), muted, &pa_context_set_source_output_mute);
}
