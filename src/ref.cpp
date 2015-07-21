#include "ref.h"

#include "context.h"

#include <QObject>

namespace QPulseAudio
{

Ref::Ref()
{
    auto c = Context::instance();
    c->ref();
}

Ref::~Ref()
{
    Context::instance()->unref();
}

Context *Ref::context() const
{
    return Context::instance();
}

} // QPulseAudio
