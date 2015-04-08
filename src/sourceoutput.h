#ifndef SOURCEOUTPUT_H
#define SOURCEOUTPUT_H

#include "streambase.h"

class Q_DECL_EXPORT SourceOutput : public StreamBase<pa_source_output_info>
{
    Q_OBJECT
public:
    void setInfo(const pa_source_output_info *info) Q_DECL_OVERRIDE Q_DECL_FINAL;

    quint32 sourceIndex() const { return m_sourceIndex; }

private:
    quint32 m_sourceIndex = 0;
};

#endif // SOURCEOUTPUT_H
