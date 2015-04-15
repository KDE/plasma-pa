#ifndef REF_H
#define REF_H

#include <QtGlobal>

class Context;

class Q_DECL_EXPORT Ref
{
public:
    Ref();
    virtual ~Ref();

    Context *context() const;
};

#endif // REF_H
