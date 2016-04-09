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

#include "streamrestore.h"
#include "context.h"
#include "debug.h"

namespace QPulseAudio
{

StreamRestore::StreamRestore(quint32 index, const QVariantMap &properties, QObject *parent)
    : PulseObject(parent)
    , m_muted(false)
{
    m_index = index;
    m_properties = properties;
}

void StreamRestore::update(const pa_ext_stream_restore_info *info)
{
    const QString infoName = QString::fromUtf8(info->name);
    if (m_name != infoName) {
        m_name = infoName;
        emit nameChanged();
    }
    const QString infoDevice = QString::fromUtf8(info->device);
    if (m_device != infoDevice) {
        m_device = infoDevice;
        emit deviceChanged();
    }
    if (m_muted != info->mute) {
        m_muted = info->mute;
        emit mutedChanged();
    }
    if (memcmp(&m_volume, &info->volume, sizeof(pa_cvolume)) != 0) {
        m_volume = info->volume;
        emit volumeChanged();
        emit channelVolumesChanged();
    }
    if (memcmp(&m_channelMap, &info->channel_map, sizeof(pa_channel_map)) != 0) {
        m_channels.clear();
        m_channels.reserve(info->channel_map.channels);
        for (int i = 0; i < info->channel_map.channels; ++i) {
            m_channels << QString::fromUtf8(pa_channel_position_to_pretty_string(info->channel_map.map[i]));
        }
        m_channelMap = info->channel_map;
        emit channelsChanged();
    }
}

QString StreamRestore::name() const
{
    return m_name;
}

QString StreamRestore::device() const
{
    return m_device;
}

void StreamRestore::setDevice(const QString &device)
{
    writeChanges(m_volume, m_channelMap, m_muted, device);
}

qint64 StreamRestore::volume() const
{
    return m_volume.values[0];
}

void StreamRestore::setVolume(qint64 volume)
{
    pa_cvolume vol = m_volume;
    for (int i = 0; i < vol.channels; ++i) {
        vol.values[i] = volume;
    }
    writeChanges(vol, m_channelMap, m_muted, m_device);
}

bool StreamRestore::isMuted() const
{
    return m_muted;
}

void StreamRestore::setMuted(bool muted)
{
    writeChanges(m_volume, m_channelMap, muted, m_device);
}

bool StreamRestore::hasVolume() const
{
    return true;
}

bool StreamRestore::isVolumeWritable() const
{
    return true;
}

QStringList StreamRestore::channels() const
{
    return m_channels;
}

QList<qint64> StreamRestore::channelVolumes() const
{
    QList<qint64> ret;
    ret.reserve(m_volume.channels);
    for (int i = 0; i < m_volume.channels; ++i) {
        ret << m_volume.values[i];
    }
    return ret;
}

void StreamRestore::setChannelVolume(int channel, qint64 volume)
{
    Q_ASSERT(channel >= 0 && channel < m_volume.channels);
    pa_cvolume vol = m_volume;
    vol.values[channel] = volume;
    writeChanges(vol, m_channelMap, m_muted, m_device);
}

quint32 StreamRestore::deviceIndex() const
{
    return PA_INVALID_INDEX;
}

void StreamRestore::setDeviceIndex(quint32 deviceIndex)
{
    Q_UNUSED(deviceIndex);
    qCWarning(PLASMAPA) << "Not implemented";
}

void StreamRestore::writeChanges(const pa_cvolume &volume, const pa_channel_map &channelMap, bool muted, const QString &device)
{
    const QByteArray nameData = m_name.toUtf8();
    const QByteArray deviceData = device.toUtf8();

    pa_ext_stream_restore_info info;
    info.name = nameData.constData();
    info.channel_map = channelMap;
    info.volume = volume;
    info.device = deviceData.isEmpty() ? nullptr : deviceData.constData();
    info.mute = muted;

    context()->streamRestoreWrite(&info);
}

} // QPulseAudio
