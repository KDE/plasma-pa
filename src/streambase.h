#ifndef STREAMBASE_H
#define STREAMBASE_H

#include <QString>

#include <pulse/volume.h>

#include "abstractvolumebase.h"
#include "pulseobject.h"

#include "context.h"
// Properties need fully qualified classes even with pointers.
#include "client.h"

class Q_DECL_EXPORT StreamBase : public VolumeObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(bool hasVolume READ hasVolume NOTIFY hasVolumeChanged)
    Q_PROPERTY(bool volumeWritable READ isVolumeWritable NOTIFY isVolumeWritableChanged)
    Q_PROPERTY(Client *client READ client NOTIFY clientChanged)
public:
    template <typename PAInfo>
    void updateStream(const PAInfo *info)
    {
        updateVolumeObject(info);

        if (m_name != QString::fromUtf8(info->name)) {
            m_name = QString::fromUtf8(info->name);
            emit nameChanged();
        }
        if (m_hasVolume != info->has_volume) {
            m_hasVolume = info->has_volume;
            emit hasVolumeChanged();
        }
        if (m_isVolumeWritable != info->volume_writable) {
            m_isVolumeWritable = info->volume_writable;
            emit isVolumeWritableChanged();
        }
        if (m_clientIndex != info->client) {
            m_clientIndex = info->client;
            emit clientChanged();
        }
    }

    QString name() const { return m_name; }
    bool hasVolume() const { return m_hasVolume; }
    bool isVolumeWritable() const { return m_isVolumeWritable; }
    Client *client() const
    {
        qDebug() <<"client";
        return context()->clients().data().value(m_clientIndex, nullptr);
    }

signals:
    void nameChanged();
    void hasVolumeChanged();
    void isVolumeWritableChanged();
    void clientChanged();

protected:
    StreamBase() {}
    virtual ~StreamBase() {}

private:
    QString m_name;
    bool m_hasVolume;
    bool m_isVolumeWritable;
    quint32 m_clientIndex;
};

#endif // STREAMBASE_H
