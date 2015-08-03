/*
    Copyright 2014-2015 Harald Sitter <sitter@kde.org>

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

#ifndef VOLUMEOBJECT_H
#define VOLUMEOBJECT_H

#include <pulse/volume.h>

#include "pulseobject.h"

namespace QPulseAudio
{

class VolumeObject : public PulseObject
{
    Q_OBJECT
    Q_PROPERTY(qint64 volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(bool muted READ isMuted WRITE setMuted NOTIFY mutedChanged)
    Q_PROPERTY(bool hasVolume READ hasVolume NOTIFY hasVolumeChanged)
    Q_PROPERTY(bool volumeWritable READ isVolumeWritable NOTIFY isVolumeWritableChanged)
public:
    VolumeObject(QObject *parent);
    virtual ~VolumeObject();

    template <typename PAInfo>
    void updateVolumeObject(PAInfo *info)
    {
        updatePulseObject(info);
        if (m_muted != info->mute) {
            m_muted = info->mute;
            emit mutedChanged();
        }
#warning fixme volume needs manual compare
//            if (m_volume != info->volume) {
                m_volume = info->volume;
                emit volumeChanged();
//            }
    }

    qint64 volume() const;
    virtual void setVolume(qint64 volume) = 0;

    bool isMuted() const;
    virtual void setMuted(bool muted) = 0;

    bool hasVolume() const;
    bool isVolumeWritable() const;

signals:
    void volumeChanged();
    void mutedChanged();
    void hasVolumeChanged();
    void isVolumeWritableChanged();

protected:
    pa_cvolume cvolume() const;

    pa_cvolume m_volume;
    bool m_muted;
    bool m_hasVolume;
    bool m_volumeWritable;
};

} // QPulseAudio

#endif // VOLUMEOBJECT_H
