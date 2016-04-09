/*
    Copyright 2016 David Rosca <nowrep@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef STREAMRESTORE_H
#define STREAMRESTORE_H

#include "pulseobject.h"

#include <pulse/ext-stream-restore.h>

namespace QPulseAudio
{

class Q_DECL_EXPORT StreamRestore : public PulseObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString device READ device WRITE setDevice NOTIFY deviceChanged)
    Q_PROPERTY(qint64 volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(bool muted READ isMuted WRITE setMuted NOTIFY mutedChanged)
    Q_PROPERTY(bool hasVolume READ hasVolume CONSTANT)
    Q_PROPERTY(bool volumeWritable READ isVolumeWritable CONSTANT)
    Q_PROPERTY(QStringList channels READ channels NOTIFY channelsChanged)
    Q_PROPERTY(QList<qint64> channelVolumes READ channelVolumes NOTIFY channelVolumesChanged)
    Q_PROPERTY(quint32 deviceIndex READ deviceIndex WRITE setDeviceIndex NOTIFY deviceIndexChanged)
public:
    StreamRestore(quint32 index, const QVariantMap &properties, QObject *parent);

    void update(const pa_ext_stream_restore_info *info);

    QString name() const;

    QString device() const;
    void setDevice(const QString &device);

    qint64 volume() const;
    void setVolume(qint64 volume);

    bool isMuted() const;
    void setMuted(bool muted);

    bool hasVolume() const;
    bool isVolumeWritable() const;

    QStringList channels() const;

    QList<qint64> channelVolumes() const;

    quint32 deviceIndex() const;
    void setDeviceIndex(quint32 deviceIndex);

    Q_INVOKABLE void setChannelVolume(int channel, qint64 volume);

signals:
    void nameChanged();
    void deviceChanged();
    void volumeChanged();
    void mutedChanged();
    void channelsChanged();
    void channelVolumesChanged();
    void deviceIndexChanged();

private:
    void writeChanges(const pa_cvolume &volume, const pa_channel_map &channelMap, bool muted, const QString &device);

    QString m_name;
    QString m_device;
    pa_cvolume m_volume;
    pa_channel_map m_channelMap;
    QStringList m_channels;
    bool m_muted;
};

} // QPulseAudio

#endif // STREAMRESTORE_H
