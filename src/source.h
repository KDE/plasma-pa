#ifndef SOURCE_H
#define SOURCE_H

#include <pulse/introspect.h>

#include "pulseobject.h"

class Q_DECL_EXPORT Source : public PulseObject
{
    Q_OBJECT
public:
    Source();
    void setInfo(const pa_source_info *info);

    QString name() { return m_name; }
    QString description() { return m_description; }
    pa_cvolume volume() const { return m_volume; }
    bool isMuted() const { return m_isMuted; }

private:
   QString m_name;
   QString m_description;
   pa_cvolume m_volume;
   bool m_isMuted;
};

#endif // SOURCE_H
