#include "sinkinput.h"

#include "context.h"

namespace QPulseAudio
{

SinkInput::SinkInput(QObject *parent)
    : Stream(parent)
{
}

void SinkInput::update(const pa_sink_input_info *info)
{
    updateStream(info);
    if (m_sinkIndex != info->sink) {
        m_sinkIndex = info->sink;
        emit sinkIndexChanged();
    }
}

quint32 SinkInput::sinkIndex() const
{
    return m_sinkIndex;
}

void SinkInput::setSinkIndex(quint32 sinkIndex)
{
    context()->setGenericDeviceForStream(index(), sinkIndex, &pa_context_move_sink_input_by_index);
}

void SinkInput::setVolume(qint64 volume)
{
    context()->setGenericVolume(index(), volume, cvolume(), &pa_context_set_sink_input_volume);
}

void SinkInput::setMuted(bool muted)
{
    context()->setGenericMute(index(), muted, &pa_context_set_sink_input_mute);
}

} // QPulseAudio
