/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef VOLUMEOBJECT_H
#define VOLUMEOBJECT_H

#include "indexedpulseobject.h"

namespace PulseAudioQt
{
/**
 * An PulseObject that has a volume. Can be a Device or a Stream.
 */
class PULSEAUDIOQT_EXPORT VolumeObject : public IndexedPulseObject
{
    Q_OBJECT
    Q_PROPERTY(qint64 volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(bool muted READ isMuted WRITE setMuted NOTIFY mutedChanged)
    Q_PROPERTY(bool volumeWritable READ isVolumeWritable NOTIFY isVolumeWritableChanged)
    Q_PROPERTY(QVector<QString> channels READ channels NOTIFY channelsChanged)
    Q_PROPERTY(QVector<qint64> channelVolumes READ channelVolumes WRITE setChannelVolumes NOTIFY channelVolumesChanged)
    Q_PROPERTY(QStringList rawChannels READ rawChannels NOTIFY rawChannelsChanged)

public:
    ~VolumeObject();

    /**
     * This object's volume
     */
    qint64 volume() const;

    /**
     * Set the volume for this object.
     * This affects all channels.
     * The volume must be between PulseAudioQt::minimumVolume() and PulseAudioQt::maximumVolume().
     */
    virtual void setVolume(qint64 volume) = 0;

    /**
     * Whether this object is muted.
     */
    bool isMuted() const;

    /**
     * Set whether this object is muted.
     */
    virtual void setMuted(bool muted) = 0;

    bool isVolumeWritable() const;

    QVector<QString> channels() const;
    QStringList rawChannels() const;
    QVector<qint64> channelVolumes() const;
    virtual void setChannelVolumes(const QVector<qint64> &channelVolumes) = 0;
    Q_INVOKABLE virtual void setChannelVolume(int channel, qint64 volume) = 0;

Q_SIGNALS:
    void volumeChanged();
    void mutedChanged();
    void isVolumeWritableChanged();
    void channelsChanged();
    void rawChannelsChanged();
    void channelVolumesChanged();

protected:
    /** @private */
    explicit VolumeObject(QObject *parent);
    /** @private */
    class VolumeObjectPrivate *const d;
    friend class DevicePrivate;
    friend class StreamPrivate;
};

} // PulseAudioQt

#endif // VOLUMEOBJECT_H
