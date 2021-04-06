/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef SINKINPUT_H
#define SINKINPUT_H

#include "stream.h"

namespace QPulseAudio
{
class SinkInput : public Stream
{
    Q_OBJECT
public:
    explicit SinkInput(QObject *parent);

    void update(const pa_sink_input_info *info);

    void setSinkIndex(quint32 sinkIndex);

    void setVolume(qint64 volume) override;
    void setMuted(bool muted) override;
    void setChannelVolume(int channel, qint64 volume) override;
    void setChannelVolumes(const QVector<qint64> &channelVolumes) override;
    void setDeviceIndex(quint32 deviceIndex) override;

    quint32 sourceIndex() const override;
    quint32 streamIndex() const override;

private:
    quint32 m_sinkIndex = -1;
};

} // QPulseAudio

#endif // SINKINPUT_H
