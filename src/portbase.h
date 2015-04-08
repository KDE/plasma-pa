#ifndef PORTBASE_H
#define PORTBASE_H

#include "profilebase.h"

template <typename PAInfo>
class Q_DECL_EXPORT PortBase : public ProfileBase<PAInfo>
{
public:
    PortBase()
        : ProfileBase<PAInfo>()
        , m_isAvailable(false)
    {
    }

    virtual ~PortBase() {}

    virtual void setInfo(const PAInfo *info) Q_DECL_OVERRIDE
    {
        ProfileBase<PAInfo>::setInfo(info);
        m_isAvailable = info->available;
    }

    bool isAvailable() const { return m_isAvailable; }

    virtual QVariantMap toVariantMap() const Q_DECL_OVERRIDE Q_DECL_FINAL
    {
        auto map = ProfileBase<PAInfo>::toVariantMap();
        map.insert(QLatin1Literal("available"), isAvailable());
        return map;
    }

private:
    bool m_isAvailable;
};

#endif // PORTBASE_H
