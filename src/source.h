#ifndef SOURCE_H
#define SOURCE_H

#include <pulse/introspect.h>

#include "portbase.h"
#include "pulseobject.h"

class Q_DECL_EXPORT Source : public PulseObject
{
    Q_OBJECT
public:
    typedef PortBase<pa_source_port_info> Port;

    Source();
    void setInfo(const pa_source_info *info);

    QString name() { return m_name; }
    QString description() { return m_description; }
    pa_cvolume volume() const { return m_volume; }
    bool isMuted() const { return m_isMuted; }
    QList<Port> ports() const { return m_ports; }
    int activePortIndex() const { return m_activePortIndex; }

private:
   QString m_name;
   QString m_description;
   pa_cvolume m_volume;
   bool m_isMuted;
   QList<Port> m_ports;
   int m_activePortIndex;
};

#endif // SOURCE_H
