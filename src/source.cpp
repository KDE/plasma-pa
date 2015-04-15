#include "source.h"

#include "context.h"

void Source::setVolume(qint64 volume)
{
    s_context->setSourceVolume(index(), volume);
}

void Source::setMuted(bool muted)
{
    s_context->setGenericMute(index(), muted, &pa_context_set_source_mute_by_index);
}
