/*
    Copyright 2020 David Edmundson <davidedmundson@kde.org>

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

#include "volumemonitor.h"

#include <pulse/pulseaudio.h>

#include "context.h"
#include "debug.h"
#include "volumeobject.h"

#include <QtGlobal>

using namespace QPulseAudio;

VolumeMonitor::VolumeMonitor(QObject* parent)
    : QObject(parent)
{
    Context::instance()->ref();
}

VolumeMonitor::~VolumeMonitor()
{
    setTarget(nullptr);
    Context::instance()->unref();
}

bool VolumeMonitor::isAvailable() const
{
    return m_stream != nullptr;
}

void VolumeMonitor::updateVolume(qreal volume)
{
    // qFuzzyCompare cannot compare against 0.
    if (qFuzzyCompare(1 + m_volume, 1 + volume)) {
        return;
    }

    m_volume = volume;
    Q_EMIT volumeChanged();
}

QPulseAudio::VolumeObject * QPulseAudio::VolumeMonitor::target() const
{
    return m_target.data();
}

void QPulseAudio::VolumeMonitor::setTarget(QPulseAudio::VolumeObject* target)
{
    if (target == m_target) {
        return;
    }

    if (m_stream) {
        pa_stream_set_read_callback(m_stream, nullptr, nullptr);
        pa_stream_set_suspended_callback(m_stream, nullptr, nullptr);
        pa_stream_disconnect(m_stream);
        pa_stream_unref(m_stream);
        m_stream = nullptr;
        Q_EMIT availableChanged();
    }

    m_target = target;

    if (target) {
        createStream();
    }

    Q_EMIT targetChanged();
}

void VolumeMonitor::createStream()
{
    Q_ASSERT(!m_stream);

    uint32_t sourceIdx = m_target->sourceIndex();
    uint32_t streamIdx = m_target->streamIndex();

    if (sourceIdx == PA_INVALID_INDEX) {
        return;
    }

    char t[16];
    pa_buffer_attr attr;
    pa_sample_spec ss;
    pa_stream_flags_t flags;

    ss.channels = 1;
    ss.format = PA_SAMPLE_FLOAT32;
    ss.rate = 25;

    memset(&attr, 0, sizeof(attr));
    attr.fragsize = sizeof(float);
    attr.maxlength = (uint32_t) -1;

    snprintf(t, sizeof(t), "%u", sourceIdx);

    if (!(m_stream = pa_stream_new(Context::instance()->context(), "PlasmaPA-VolumeMeter", &ss, nullptr))) {
        qCWarning(PLASMAPA) << "Failed to create stream";
        return;
    }

    if (streamIdx != PA_INVALID_INDEX) {
        pa_stream_set_monitor_stream(m_stream, streamIdx);
    }

    pa_stream_set_read_callback(m_stream, read_callback, this);
    pa_stream_set_suspended_callback(m_stream, suspended_callback, this);

    flags = (pa_stream_flags_t) (PA_STREAM_DONT_MOVE | PA_STREAM_PEAK_DETECT | PA_STREAM_ADJUST_LATENCY);

    if (pa_stream_connect_record(m_stream, t, &attr, flags) < 0) {
        pa_stream_unref(m_stream);
        m_stream = nullptr;
        return;
    }
    Q_EMIT availableChanged();
}

void VolumeMonitor::suspended_callback(pa_stream *s, void *userdata)
{
    VolumeMonitor *w = static_cast<VolumeMonitor*>(userdata);
    if (pa_stream_is_suspended(s)) {
            w->updateVolume(-1);
    }
}

void VolumeMonitor::read_callback(pa_stream *s, size_t length, void *userdata) {
    VolumeMonitor *w = static_cast<VolumeMonitor*>(userdata);
    const void *data;
    double volume;

    if (pa_stream_peek(s, &data, &length) < 0) {
        qCWarning(PLASMAPA) << "Failed to read data from stream";
        return;
    }

    if (!data) {
        /* nullptr data means either a hole or empty buffer.
         * Only drop the stream when there is a hole (length > 0) */
        if (length) {
            pa_stream_drop(s);
        }
        return;
    }

    Q_ASSERT(length > 0);
    Q_ASSERT(length % sizeof(float) == 0);

    volume = ((const float*) data)[length / sizeof(float) -1];

    pa_stream_drop(s);

    volume = qBound(0.0, volume, 1.0);
    w->updateVolume(volume);
}
