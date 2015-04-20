#ifndef PROFILE_H
#define PROFILE_H

#include <QObject>
#include <QString>

class Q_DECL_EXPORT Profile : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString description READ description NOTIFY descriptionChanged)
    Q_PROPERTY(quint32 priority READ priority NOTIFY priorityChanged)
public:
    Profile(QObject *parent);
    virtual ~Profile();

    template<typename PAInfo>
    void setInfo(const PAInfo *info)
    {
        // Description is optional. Name not so much as we need some ID.
        Q_ASSERT(info->name);
        QString infoName = QString::fromUtf8(info->name);
        if (m_name != infoName) {
            m_name = infoName;
            emit nameChanged();
        }
        if (info->description) {
            QString infoDescription = QString::fromUtf8(info->description);
            if (m_description != infoDescription) {
                m_description = infoDescription;
                emit descriptionChanged();
            }
        }
        if (m_priority != info->priority) {
            m_priority = info->priority;
            emit priorityChanged();
        }
    }

    QString name() const;
    QString description() const;
    quint32 priority() const;

signals:
    void nameChanged();
    void descriptionChanged();
    void priorityChanged();

private:
    QString m_name;
    QString m_description;
    quint32 m_priority;
};

#endif // PROFILE_H
