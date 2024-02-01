/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef STREAM_H
#define STREAM_H

#include <QString>

#include "volumeobject.h"

// Properties need fully qualified classes even with pointers.
#include "client.h"

namespace PulseAudioQt
{
class StreamPrivate;

class PULSEAUDIOQT_EXPORT Stream : public VolumeObject
{
    Q_OBJECT
    Q_PROPERTY(PulseAudioQt::Client *client READ client NOTIFY clientChanged)
    Q_PROPERTY(bool virtualStream READ isVirtualStream NOTIFY virtualStreamChanged)
    Q_PROPERTY(quint32 deviceIndex READ deviceIndex WRITE setDeviceIndex NOTIFY deviceIndexChanged)
    Q_PROPERTY(bool corked READ isCorked NOTIFY corkedChanged)
    Q_PROPERTY(bool hasVolume READ hasVolume NOTIFY hasVolumeChanged)

public:
    ~Stream();

    Client *client() const;
    bool isVirtualStream() const;
    quint32 deviceIndex() const;
    bool isCorked() const;
    bool hasVolume() const;

    virtual void setDeviceIndex(quint32 deviceIndex) = 0;

Q_SIGNALS:
    void clientChanged();
    void virtualStreamChanged();
    void deviceIndexChanged();
    void corkedChanged();
    void hasVolumeChanged();

protected:
    /** @private */
    explicit Stream(QObject *parent);
    /** @private */
    class StreamPrivate *const d;

    friend class SinkInputPrivate;
    friend class SourceOutputPrivate;
};

} // PulseAudioQt

#endif // STREAM_H
