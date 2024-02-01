/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef SOURCE_H
#define SOURCE_H

#include "device.h"

struct pa_source_info;

namespace PulseAudioQt
{
/**
 * A PulseAudio source. This class is based on https://freedesktop.org/software/pulseaudio/doxygen/structpa__source__info.html.
 */
class PULSEAUDIOQT_EXPORT Source : public Device
{
    Q_OBJECT

public:
    ~Source();

    void setVolume(qint64 volume) override;
    void setMuted(bool muted) override;
    void setActivePortIndex(quint32 port_index) override;
    void setChannelVolume(int channel, qint64 volume) override;
    void setChannelVolumes(const QVector<qint64> &volumes) override;

    bool isDefault() const override;
    void setDefault(bool enable) override;

    void switchStreams() override;

private:
    explicit Source(QObject *parent);

    class SourcePrivate *const d;
    friend class MapBase<Source, pa_source_info>;
};

} // PulseAudioQt

#endif // SOURCE_H
