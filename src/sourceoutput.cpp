#include "sourceoutput.h"

#include "context.h"

SourceOutput::SourceOutput(QObject *parent)
    : Stream(parent)
{
}

void SourceOutput::update(const pa_source_output_info *info)
{
    updateStream(info);
    if (m_sourceIndex != info->source) {
        m_sourceIndex = info->source;
        emit sourceIndexChanged();
    }
}

quint32 SourceOutput::sourceIndex() const
{
    return m_sourceIndex;
}

void SourceOutput::setSourceIndex(quint32 sourceIndex)
{
    context()->setGenericDeviceForStream(index(), sourceIndex, &pa_context_move_source_output_by_index);
}

void SourceOutput::setVolume(qint64 volume)
{
    context()->setGenericVolume(index(), volume, cvolume(), &pa_context_set_source_output_volume);
}

void SourceOutput::setMuted(bool muted)
{
    context()->setGenericMute(index(), muted, &pa_context_set_source_output_mute);
}
