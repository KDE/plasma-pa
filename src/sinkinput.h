#ifndef SINKINPUT_H
#define SINKINPUT_H

#include "pulseobject.h"

class SinkInput : public PulseObject
{
    Q_OBJECT
public:
    SinkInput();

    QString name() const { return m_name; }
    void setName(const char *name) { m_name = QString::fromUtf8(name); }

private:
    QString m_name;
};

#endif // SINKINPUT_H
