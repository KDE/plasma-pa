#ifndef SOURCEOUTPUT_H
#define SOURCEOUTPUT_H

#include "stream.h"

namespace QPulseAudio
{

class Q_DECL_EXPORT SourceOutput : public Stream
{
    Q_OBJECT
    Q_PROPERTY(quint32 sourceIndex READ sourceIndex WRITE setSourceIndex NOTIFY sourceIndexChanged)
public:
    SourceOutput(QObject *parent);

    void update(const pa_source_output_info *info);

    quint32 sourceIndex() const;
    void setSourceIndex(quint32 sourceIndex);

    void setVolume(qint64 volume) Q_DECL_OVERRIDE;
    void setMuted(bool muted) Q_DECL_OVERRIDE;

signals:
    void sourceIndexChanged();

private:
    quint32 m_sourceIndex = 0;
};

} // QPulseAudio

#endif // SOURCEOUTPUT_H
