#ifndef PULSEOBJECT_H
#define PULSEOBJECT_H

#include <QDebug>
#include <QObject>

#include <pulse/introspect.h>

class Context;

class Q_DECL_EXPORT PulseObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(quint32 index READ index CONSTANT)
    Q_PROPERTY(QVariantMap properties READ properties NOTIFY propertiesChanged)
public:
    template <typename PAInfo>
    void updatePulseObject(PAInfo *info)
    {
        m_index = info->index;

        m_properties.clear();
        void *it = nullptr;
        while (const char *key = pa_proplist_iterate(info->proplist, &it)) {
            Q_ASSERT(key);
            const char *value = pa_proplist_gets(info->proplist, key);
            if (!value) {
                qDebug() << "property" << key << "not a string";
                continue;
            }
            Q_ASSERT(value);
            m_properties.insert(QString::fromUtf8(key), QString::fromUtf8(value));
        }
        emit propertiesChanged();
    }

    quint32 index() const;
    QVariantMap properties() const;

signals:
    void propertiesChanged();

protected:
    PulseObject(QObject *parent);
    virtual ~PulseObject();

    Context *context() const;
    quint32 m_index;
    QVariantMap m_properties;

private:
    // Ensure that we get properly parented.
    PulseObject();
};

#endif // PULSEOBJECT_H
