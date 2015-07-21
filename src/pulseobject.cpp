#include "pulseobject.h"

#include "context.h"

namespace QPulseAudio
{

PulseObject::PulseObject(QObject *parent)
    : QObject(parent)
    , m_index(0)
{
}

PulseObject::~PulseObject()
{
}

Context *PulseObject::context() const
{
    return Context::instance();
}

uint32_t PulseObject::index() const
{
    return m_index;
}

QVariantMap PulseObject::properties() const
{
    return m_properties;
}

} // QPulseAudio
