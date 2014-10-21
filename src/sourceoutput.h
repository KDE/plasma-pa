#ifndef SOURCEOUTPUT_H
#define SOURCEOUTPUT_H

#include <pulse/introspect.h>

#include "pulseobject.h"

class SourceOutput : public PulseObject
{
    Q_OBJECT
public:
    SourceOutput();
    void setInfo(const pa_source_output_info *info);

    QString name() const { return m_name; }
    quint32 sourceIndex() const { return m_sourceIndex; }
    pa_cvolume volume() const { return m_volume; }
    bool isMuted() const { return m_isMuted; }

    quint32 client() const { return m_client; }

private:
   QString m_name;
   quint32 m_sourceIndex;
   pa_cvolume m_volume;
   bool m_isMuted;
   quint32 m_client;
};

#endif // SOURCEOUTPUT_H
