/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef SOURCEOUTPUT_H
#define SOURCEOUTPUT_H

#include "stream.h"

struct pa_source_output_info;

namespace PulseAudioQt
{
/**
 * A SourceOutput Stream.
 */
class PULSEAUDIOQT_EXPORT SourceOutput : public Stream
{
    Q_OBJECT

public:
    ~SourceOutput();

    void setVolume(qint64 volume) override;
    void setMuted(bool muted) override;
    void setChannelVolume(int channel, qint64 volume) override;
    void setDeviceIndex(quint32 deviceIndex) override;
    void setChannelVolumes(const QVector<qint64> &channelVolumes) override;

private:
    explicit SourceOutput(QObject *parent);

    class SourceOutputPrivate *const d;
    friend class MapBase<SourceOutput, pa_source_output_info>;
};

} // PulseAudioQt

#endif // SOURCEOUTPUT_H
