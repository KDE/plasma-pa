#include "sink.h"

#include "context.h"

void Sink::setVolume(qint64 volume)
{
    s_context->setSinkVolume(index(), volume);
}

void Sink::setMuted(bool muted)
{
    s_context->setGenericMute(m_index, muted, &pa_context_set_sink_mute_by_index);
}
