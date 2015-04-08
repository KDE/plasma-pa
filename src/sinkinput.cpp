#include "sinkinput.h"

void SinkInput::setInfo(const pa_sink_input_info *info)
{
    StreamBase<pa_sink_input_info>::setInfo(info);
    m_sinkIndex = info->sink;
}
