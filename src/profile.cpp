#include "profile.h"

namespace QPulseAudio
{

Profile::Profile(QObject *parent)
    : QObject(parent)
    , m_name()
    , m_description()
    , m_priority(0)
{
}

Profile::~Profile()
{
}

QString Profile::name() const
{
    return m_name;
}

QString Profile::description() const
{
    return m_description;
}

quint32 Profile::priority() const
{
    return m_priority;
}

} // QPulseAudio
