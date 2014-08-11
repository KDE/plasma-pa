#ifndef PULSEOBJECT_H
#define PULSEOBJECT_H

#include <QObject>

#include <pulse/introspect.h>

class PulseObject : public QObject
{
public:
    quint32 index() const;

protected:
    PulseObject(QObject *parent = 0);
    virtual ~PulseObject();

//    virtual void setProplist(const pa_proplist *proplist);

    quint32 m_index;
};

#endif // PULSEOBJECT_H
