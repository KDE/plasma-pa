#ifndef STREAM_H
#define STREAM_H

#include <QString>

#include <pulse/volume.h>

#include "volumeobject.h"
#include "pulseobject.h"

#include "context.h"
// Properties need fully qualified classes even with pointers.
#include "client.h"

class Q_DECL_EXPORT Stream : public VolumeObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
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
        if (m_volumeWritable != info->volume_writable) {
            m_volumeWritable = info->volume_writable;
            emit isVolumeWritableChanged();
        }
        if (m_clientIndex != info->client) {
            m_clientIndex = info->client;
            emit clientChanged();
        }
    }

    QString name() const;
    Client *client() const;

signals:
    void nameChanged();
    void clientChanged();

protected:
    Stream(QObject *parent);
    virtual ~Stream();

private:
    QString m_name;
    quint32 m_clientIndex;
};

#endif // STREAM_H
