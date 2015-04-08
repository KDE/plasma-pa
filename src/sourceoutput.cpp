#include "sourceoutput.h"

void SourceOutput::setInfo(const pa_source_output_info *info)
{
    m_sourceIndex = info->source;
}
