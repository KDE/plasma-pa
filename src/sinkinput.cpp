#include "sinkinput.h"

#include "context.h"

void SinkInput::update(const pa_sink_input_info *info)
{
    updateStream(info);
    m_sinkIndex = info->sink;
}

void SinkInput::setVolume(qint64 volume)
{
    context()->setGenericVolume(index(), volume, cvolume(), &pa_context_set_sink_input_volume);
}

void SinkInput::setMuted(bool muted)
{
    context()->setGenericMute(index(), muted, &pa_context_set_sink_input_mute);
}
