/*
    SPDX-FileCopyrightText: 2026 Ramil Nurmanov <ramil2004nur@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "microphonetest.h"
#include <PulseAudioQt/Context>
#include <cmath>
#include <pulse/error.h>
#include <chrono>
#include <pulse/volume.h>
#include <KLocalizedString>

namespace
{
    constexpr int SAMPLE_RATE = 44100;
    constexpr std::chrono::milliseconds MAX_RECORD_DURATION{10000};
    constexpr int MIN_FRAGMENT_SIZE = 256;
}

MicrophoneTest::MicrophoneTest(QObject *parent)
    : QObject(parent)
{
    m_autoStopTimer.setSingleShot(true);
    connect(&m_autoStopTimer, &QTimer::timeout, this, [this]() {
        if (m_recording && m_recordStream && !m_manualStop) {
            stopRecording();
        }
    });
}

MicrophoneTest::~MicrophoneTest()
{
    cleanupStreams();
}

double MicrophoneTest::volumeLevel() const
{
    return m_volumeLevel;
}

PulseAudioQt::Source *MicrophoneTest::source() const
{
    return m_source;
}

void MicrophoneTest::setSource(PulseAudioQt::Source *source)
{
    if (m_source != source) {
        cleanupStreams();
        m_source = source;
        m_recordedData.clear();
        m_playbackOffset = 0;
        m_manualStop = false;
        m_volumeLevel = 0.0;
        Q_EMIT sourceChanged();
        Q_EMIT hasRecordingChanged();
        Q_EMIT volumeLevelChanged();
    }
}

bool MicrophoneTest::hasRecording() const
{
    return !m_recordedData.isEmpty();
}

bool MicrophoneTest::playing() const
{
    return m_playing;
}

bool MicrophoneTest::recording() const
{
    return m_recording;
}

void MicrophoneTest::calculateVolumeLevel(const void *data, size_t nbytes)
{
    const int16_t *samples = static_cast<const int16_t *>(data);
    size_t sampleCount = nbytes / sizeof(int16_t);
    if (sampleCount == 0) {
        m_volumeLevel = 0.0;
        Q_EMIT volumeLevelChanged();
        return;
    }
    double sum = 0.0;
    for (size_t i = 0; i < sampleCount; ++i) {
        double normalized = static_cast<double>(samples[i]) / PA_VOLUME_NORM;
        sum += normalized * normalized;
    }
    double rms = std::sqrt(sum / sampleCount);
    pa_volume_t pulseVolume = pa_sw_volume_from_linear(rms);
    m_volumeLevel = static_cast<double>(pulseVolume) / PA_VOLUME_NORM;
    Q_EMIT volumeLevelChanged();
}

void MicrophoneTest::startRecording()
{
    if (m_recording || !m_source || m_playing) {
        return;
    }

    auto pa_ctx = PulseAudioQt::Context::instance()->context();
    if (!pa_ctx || pa_context_get_state(pa_ctx) != PA_CONTEXT_READY) {
        Q_EMIT showErrorMessage(i18n("PulseAudio context is not ready"));
        return;
    }

    pa_sample_spec ss = {PA_SAMPLE_S16LE, SAMPLE_RATE, 1};

    m_recordStream = pa_stream_new(pa_ctx, "MicTest-Record", &ss, nullptr);
    if (!m_recordStream) {
        Q_EMIT showErrorMessage(i18n("Failed to create a record stream"));
        return;
    }

    pa_stream_set_state_callback(m_recordStream, stream_state_callback, this);
    pa_stream_set_read_callback(m_recordStream, stream_read_callback, this);

    pa_buffer_attr attr = {
        .maxlength = (uint32_t)-1,
        .tlength = (uint32_t)-1,
        .prebuf = (uint32_t)-1,
        .minreq = (uint32_t)-1,
        .fragsize = MIN_FRAGMENT_SIZE};

    if (pa_stream_connect_record(m_recordStream, m_source->name().toUtf8().constData(),
        &attr, static_cast<pa_stream_flags_t>(PA_STREAM_INTERPOLATE_TIMING | PA_STREAM_ADJUST_LATENCY)) < 0) {
        Q_EMIT showErrorMessage(i18n("Failed to connect record: %1", pa_strerror(pa_context_errno(pa_ctx))));
        pa_stream_unref(m_recordStream);
        m_recordStream = nullptr;
        return;
    }

    m_recordedData.clear();
    m_playbackOffset = 0;
    m_manualStop = false;
    m_volumeLevel = 0.0;
    m_recording = true;
    m_playing = false;
    Q_EMIT recordingChanged();
    Q_EMIT volumeLevelChanged();

    m_autoStopTimer.start(MAX_RECORD_DURATION.count());
}

void MicrophoneTest::stopRecording()
{
    m_manualStop = true;

    if (m_autoStopTimer.isActive()) {
        m_autoStopTimer.stop();
    }

    if (!m_recordStream) {
        m_volumeLevel = 0.0;
        m_recording = false;
        Q_EMIT recordingChanged();
        Q_EMIT volumeLevelChanged();
        Q_EMIT hasRecordingChanged();
        Q_EMIT recordingFinished();
        return;
    }

    pa_operation *cork_op = pa_stream_cork(m_recordStream, 1, nullptr, nullptr);
    if (cork_op) {
        pa_operation_unref(cork_op);
    }

    pa_operation *drain_op = pa_stream_drain(
        m_recordStream,
        [](pa_stream *s, int success, void *userdata) {
            auto *self = static_cast<MicrophoneTest *>(userdata);
            if (!self) return;

            pa_stream_disconnect(s);
            pa_stream_unref(s);

            QMetaObject::invokeMethod(self, [self]() {
                self->m_recordStream = nullptr;
                self->m_volumeLevel = 0.0;
                self->m_recording = false;
                Q_EMIT self->recordingChanged();
                Q_EMIT self->volumeLevelChanged();
                Q_EMIT self->hasRecordingChanged();
                Q_EMIT self->recordingFinished();
                self->playRecording();
            });
        },
        this);

    if (drain_op) {
        pa_operation_unref(drain_op);
    } else {
        pa_stream_disconnect(m_recordStream);
        pa_stream_unref(m_recordStream);
        m_recordStream = nullptr;
        m_volumeLevel = 0.0;
        m_recording = false;
        Q_EMIT recordingChanged();
        Q_EMIT volumeLevelChanged();
        Q_EMIT hasRecordingChanged();
        Q_EMIT recordingFinished();
        playRecording();
    }
}

void MicrophoneTest::playRecording()
{
    if (m_recording || m_playing || m_recordedData.isEmpty()) {
        return;
    }

    auto pa_ctx = PulseAudioQt::Context::instance()->context();
    if (!pa_ctx || pa_context_get_state(pa_ctx) != PA_CONTEXT_READY) {
        Q_EMIT showErrorMessage(i18n("PulseAudio context is not ready"));
        return;
    }

    pa_sample_spec ss = {PA_SAMPLE_S16LE, SAMPLE_RATE, 1};

    m_playbackOffset = 0;

    m_playbackStream = pa_stream_new(pa_ctx, "MicTest-Playback", &ss, nullptr);
    if (!m_playbackStream) {
        Q_EMIT showErrorMessage(i18n("Couldn't create a playback stream"));
        return;
    }

    pa_stream_set_state_callback(m_playbackStream, stream_state_callback, this);
    pa_stream_set_write_callback(m_playbackStream, stream_write_callback, this);

    if (pa_stream_connect_playback(
            m_playbackStream,
            nullptr,
            nullptr,
            static_cast<pa_stream_flags_t>(
                PA_STREAM_INTERPOLATE_TIMING |
                PA_STREAM_ADJUST_LATENCY),
            nullptr,
            nullptr) < 0) {
        Q_EMIT showErrorMessage(i18n("Couldn't connect playback"));
        pa_stream_unref(m_playbackStream);
        m_playbackStream = nullptr;
        return;
    }

    m_playing = true;
    Q_EMIT playingChanged();

    pa_stream_cork(m_playbackStream, 0, nullptr, nullptr);
}

void MicrophoneTest::stopPlaying()
{
    if (!m_playing) {
        return;
    }

    if (m_playbackStream) {
        pa_stream_disconnect(m_playbackStream);
        pa_stream_unref(m_playbackStream);
        m_playbackStream = nullptr;
    }

    m_playing = false;
    Q_EMIT playingChanged();
    Q_EMIT playbackFinished();
}

void MicrophoneTest::clearRecording()
{
    m_recordedData.clear();
    m_playbackOffset = 0;
    m_volumeLevel = 0.0;
    Q_EMIT hasRecordingChanged();
    Q_EMIT volumeLevelChanged();
}

void MicrophoneTest::cleanupStreams()
{
    const bool wasRecording = m_recording;
    const bool wasPlaying = m_playing;

    if (m_recordStream) {
        pa_stream_disconnect(m_recordStream);
        pa_stream_unref(m_recordStream);
        m_recordStream = nullptr;
    }

    if (m_playbackStream) {
        pa_stream_disconnect(m_playbackStream);
        pa_stream_unref(m_playbackStream);
        m_playbackStream = nullptr;
    }

    if (m_autoStopTimer.isActive()) {
        m_autoStopTimer.stop();
    }

    m_recording = false;
    m_playing = false;
    m_manualStop = false;
    m_volumeLevel = 0.0;

    if (wasRecording) {
        Q_EMIT recordingChanged();
    }

    if (wasPlaying) {
        Q_EMIT playingChanged();
    }

    Q_EMIT volumeLevelChanged();
}

void MicrophoneTest::stream_state_callback(pa_stream *s, void *userdata)
{
    auto *self = static_cast<MicrophoneTest *>(userdata);
    pa_stream_state_t state = pa_stream_get_state(s);

    if (state == PA_STREAM_FAILED || state == PA_STREAM_TERMINATED) {
        if (s == self->m_recordStream) {
            self->m_recordStream = nullptr;
            self->m_volumeLevel = 0.0;
            Q_EMIT self->volumeLevelChanged();
        } else if (s == self->m_playbackStream) {
            self->m_playbackStream = nullptr;
            self->m_playing = false;
            Q_EMIT self->playingChanged();
            Q_EMIT self->playbackFinished();
        }
    }
}

void MicrophoneTest::stream_write_callback(pa_stream *s, size_t nbytes, void *userdata)
{
    auto *self = static_cast<MicrophoneTest *>(userdata);
    if (!self || !self->m_playbackStream) return;

    if (self->m_playbackOffset >= self->m_recordedData.size()) {
        pa_operation *op = pa_stream_drain(
            s,
            [](pa_stream *s, int, void *userdata) {
                auto *self = static_cast<MicrophoneTest *>(userdata);
                if (!self || !self->m_playbackStream) return;

                pa_stream_disconnect(s);
                pa_stream_unref(s);

                QMetaObject::invokeMethod(self, [self]() {
                    self->m_playbackStream = nullptr;
                    self->m_playing = false;
                    Q_EMIT self->playingChanged();
                    Q_EMIT self->playbackFinished();
                });
            },
            self);

        if (op) pa_operation_unref(op);
        return;
    }

    const size_t remaining = self->m_recordedData.size() - self->m_playbackOffset;
    const size_t to_write = qMin(nbytes, remaining);

    if (to_write > 0) {
        pa_stream_write(s, self->m_recordedData.constData() + self->m_playbackOffset, to_write, nullptr, 0, PA_SEEK_RELATIVE);
        self->m_playbackOffset += to_write;
    }
}

void MicrophoneTest::stream_read_callback(pa_stream *s, size_t nbytes, void *userdata)
{
    auto *self = static_cast<MicrophoneTest *>(userdata);
    const void *data;

    if (pa_stream_peek(s, &data, &nbytes) < 0 || !data) {
        return;
    }

    const bool wasEmpty = self->m_recordedData.isEmpty();

    self->m_recordedData.append(static_cast<const char *>(data), nbytes);

    self->calculateVolumeLevel(data, nbytes);

    if (wasEmpty && !self->m_recordedData.isEmpty()) {
        Q_EMIT self->hasRecordingChanged();
    }

    pa_stream_drop(s);
}