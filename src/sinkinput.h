/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef SINKINPUT_H
#define SINKINPUT_H

#include "stream.h"

struct pa_sink_input_info;

namespace PulseAudioQt
{
/**
 * A SinkInput stream.
 */
class PULSEAUDIOQT_EXPORT SinkInput : public Stream
{
    Q_OBJECT

public:
    ~SinkInput();

    void setVolume(qint64 volume) override;
    void setMuted(bool muted) override;
    void setChannelVolume(int channel, qint64 volume) override;
    void setDeviceIndex(quint32 deviceIndex) override;
    void setChannelVolumes(const QVector<qint64> &channelVolumes) override;

private:
    SinkInput(QObject *parent);

    class SinkInputPrivate *const d;
    friend class MapBase<SinkInput, pa_sink_input_info>;
};

} // PulseAudioQt

#endif // SINKINPUT_H
