#include "sourceoutput.h"

void SourceOutput::setInfo(const pa_source_output_info *info)
{
    StreamBase<pa_source_output_info>::setInfo(info);
    m_sourceIndex = info->source;
}
