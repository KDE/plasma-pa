#include "port.h"

namespace QPulseAudio
{

Port::Port(QObject *parent)
    : Profile(parent)
    , m_isAvailable(false)
{
}

Port::~Port()
{
}

bool Port::isAvailable() const
{
    return m_isAvailable;
}

} // QPulseAudio
