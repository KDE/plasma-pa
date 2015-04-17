#ifndef PORTBASE_H
#define PORTBASE_H

#include "profilebase.h"

class Q_DECL_EXPORT Port : public Profile
{
    Q_OBJECT
#warning property available
public:
    Port(QObject *parent)
        : Profile(parent)
        , m_isAvailable(false)
    {
    }

    virtual ~Port() {}

    template<typename PAInfo>
    void setInfo(const PAInfo *info)
    {
        Profile::setInfo(info);
        m_isAvailable = info->available;
    }

    bool isAvailable() const { return m_isAvailable; }

private:
    bool m_isAvailable;
};

#endif // PORTBASE_H
