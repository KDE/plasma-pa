#ifndef PROFILEBASE_H
#define PROFILEBASE_H

#include <QString>
#include <QVariantMap>

template <typename PAInfo>
class Q_DECL_EXPORT ProfileBase
{
public:
    ProfileBase()
        : m_name()
        , m_description()
        , m_priority(0)
    {
    }

    virtual ~ProfileBase() {}

    virtual void setInfo(const PAInfo *info)
    {
        // Description is optional. Name not so much as we need some ID.
        Q_ASSERT(info->name);
        m_name = QString::fromUtf8(info->name);
        if (info->description) {
            m_description = QString::fromUtf8(info->description);
        }
        m_priority = info->priority;
    }

    QString name() const { return m_name; }
    QString description() const { return m_description; }
    quint32 priority() const { return m_priority; }

    virtual QVariantMap toVariantMap() const
    {
        QVariantMap map;
        map.insert(QLatin1Literal("name"), name());
        map.insert(QLatin1Literal("description"), description());
        map.insert(QLatin1Literal("priority"), priority());
        return map;
    }

private:
    QString m_name;
    QString m_description;
    quint32 m_priority;
};

#endif // PROFILEBASE_H
