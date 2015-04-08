#include "sinkinput.h"

void SinkInput::setInfo(const pa_sink_input_info *info)
{
    m_sinkIndex = info->sink;
}
