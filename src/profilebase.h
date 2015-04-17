#ifndef PROFILEBASE_H
#define PROFILEBASE_H

#include <QString>
#include <QVariantMap>

class Q_DECL_EXPORT Profile : public QObject
{
    Q_OBJECT
#warning notify
    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(QString description READ description)
    Q_PROPERTY(quint32 priority READ priority)
public:
    Profile(QObject *parent)
        : QObject(parent)
        , m_name()
        , m_description()
        , m_priority(0)
    {
    }

    virtual ~Profile() {}

    template<typename PAInfo>
    void setInfo(const PAInfo *info)
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

private:
    QString m_name;
    QString m_description;
    quint32 m_priority;
};

#endif // PROFILEBASE_H
