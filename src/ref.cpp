#include "ref.h"

#include "context.h"

#include <QObject>

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
