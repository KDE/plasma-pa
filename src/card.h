#ifndef CARD_H
#define CARD_H

#include <pulse/introspect.h>

#include <QMap>
#include <QVariant>

#include "port.h"
#include "pulseobject.h"

class Q_DECL_EXPORT Card : public PulseObject
{
    Q_OBJECT
#warning notify
    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(QList<QObject *> profiles READ profiles)
    Q_PROPERTY(quint32 activeProfileIndex READ activeProfileIndex)
    Q_PROPERTY(QVariantMap properties READ properties)
public:
    Card(QObject *parent);

    void update(const pa_card_info *info);

    QString name() const { return m_name; }

    QList<QObject *> profiles() const { return m_profiles; }
    quint32 activeProfileIndex() const { return m_activeProfileIndex; }

    QMap<QString, QVariant> properties() const { return m_properties; }

#warning impl setting profile
//    void setCardProfile(quint32 index, const QString &profileName)
//    {
//        qDebug() << Q_FUNC_INFO << index << profileName;
//        Card *obj = m_cards.data().value(index, nullptr);
//        if (!obj)
//            return;
//        if (!PAOperation(pa_context_set_card_profile_by_index(m_context,
//                                                              index,
//                                                              profileName.toUtf8().constData(),
//                                                              nullptr, nullptr))) {
//            qWarning() << "pa_context_set_card_profile_by_index failed";
//            return;
//        }
//    }

private:
    QString m_name;

    QList<QObject *> m_profiles;
    quint32 m_activeProfileIndex;

    QVariantMap m_properties;
};

#endif // CARD_H
