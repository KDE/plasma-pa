#ifndef SOURCEOUTPUT_H
#define SOURCEOUTPUT_H

#include "stream.h"

class Q_DECL_EXPORT SourceOutput : public StreamBase
{
    Q_OBJECT
#warning missing index property
public:
    SourceOutput(QObject *parent);

    void update(const pa_source_output_info *info);

    quint32 sourceIndex() const { return m_sourceIndex; }

    void setVolume(qint64 volume) Q_DECL_OVERRIDE;
    void setMuted(bool muted) Q_DECL_OVERRIDE;

private:
    quint32 m_sourceIndex = 0;
};

#endif // SOURCEOUTPUT_H
