#ifndef SOURCEOUTPUT_H
#define SOURCEOUTPUT_H

#include "streambase.h"

class Q_DECL_EXPORT SourceOutput : public StreamBase<pa_source_output_info>
{
    Q_OBJECT
#warning missing properties

    Q_PROPERTY(qint64 volume READ volume WRITE setVolume)
    Q_PROPERTY(bool muted READ isMuted WRITE setMuted)
public:
    void setInfo(const pa_source_output_info *info) Q_DECL_OVERRIDE Q_DECL_FINAL;

    quint32 sourceIndex() const { return m_sourceIndex; }

    void setVolume(qint64 volume) Q_DECL_OVERRIDE;
    void setMuted(bool muted) Q_DECL_OVERRIDE;

private:
    quint32 m_sourceIndex = 0;
};

#endif // SOURCEOUTPUT_H
