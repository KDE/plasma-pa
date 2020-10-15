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

#pragma once

#include <QObject>
#include <QPointer>
#include <QQmlParserStatus>

struct pa_stream;


namespace QPulseAudio
{

class VolumeObject;

/**
 * This class provides a way to see the "peak" volume currently playing of any VolumeObject
 */
class VolumeMonitor : public QObject
{
    Q_OBJECT
    /**
     * Object to monitor the volume of
     * This is the "PulseObject" role of any SinkInput, Sink or Output model
     * Setting to null will stop streaming
     */
    Q_PROPERTY(QPulseAudio::VolumeObject* target READ target WRITE setTarget NOTIFY targetChanged)
    /**
     * The peak output for the volume at any given moment
     * Value is normalised between 0 and 1
     */
    Q_PROPERTY(qreal volume MEMBER m_volume NOTIFY volumeChanged)

    /**
     * Whether monitoring is available
     */
    Q_PROPERTY(bool available READ isAvailable NOTIFY availableChanged)

public:
    VolumeMonitor(QObject *parent=nullptr);
    ~VolumeMonitor();

    bool isAvailable() const;

    VolumeObject* target() const;
    void setTarget(VolumeObject *target);

Q_SIGNALS:
        void volumeChanged();
        void targetChanged();
        void availableChanged();

private:
    void createStream();
    void updateVolume(qreal volume);
    static void read_callback(pa_stream *s, size_t length, void *userdata);
    static void suspended_callback(pa_stream *s, void *userdata);

    QPointer<VolumeObject> m_target;
    pa_stream *m_stream = nullptr;

    qreal m_volume = 0;
};

}
