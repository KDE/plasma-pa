#include "pulseobject.h"

PulseObject::PulseObject(QObject *parent)
    : QObject(parent)
    , m_index(0)
{
}

PulseObject::~PulseObject()
{
}

uint32_t PulseObject::index() const
{
    return m_index;
}
