#ifndef REF_H
#define REF_H

#include <QtGlobal>

class Context;

/**
 * @brief A refcounting helper for context.
 * The Ref constructor increases the context singleton refcounter while the
 * destructor decreases the refcounter.
 * When the Context refcount reaches 0 the singleton is destroyed until it is
 * needed again. This enables us to leave no lingering context behind if the
 * user removes the plasma applet at runtime.
 */
class Q_DECL_EXPORT Ref
{
public:
    Ref();
    virtual ~Ref();

    /**
     * @brief Helper to access the context via Context::instance.
     * @return Context::instance() instance
     */
    Context *context() const;
};

#endif // REF_H
