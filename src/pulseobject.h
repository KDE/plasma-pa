#ifndef PULSEOBJECT_H
#define PULSEOBJECT_H

#include <QObject>

#include <pulse/introspect.h>

class Q_DECL_EXPORT PulseObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(quint32 index READ index CONSTANT)
public:
    quint32 index() const;

protected:
    PulseObject(QObject *parent = 0);
    virtual ~PulseObject();

//    virtual void setProplist(const pa_proplist *proplist);

    quint32 m_index;
};

#endif // PULSEOBJECT_H
