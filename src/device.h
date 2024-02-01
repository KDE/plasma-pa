/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef PA_DEVICE_H
#define PA_DEVICE_H

#include <QString>
#include <QVariantMap>

#include "port.h"
#include "volumeobject.h"

namespace PulseAudioQt
{
class Port;
class DevicePrivate;

/**
 * A PulseAudio device. Can be either a Sink or Source.
 */
class PULSEAUDIOQT_EXPORT Device : public VolumeObject
{
    Q_OBJECT
    Q_PROPERTY(State state READ state NOTIFY stateChanged)
    Q_PROPERTY(QString description READ description NOTIFY descriptionChanged)
    Q_PROPERTY(QString formFactor READ formFactor NOTIFY formFactorChanged)
    Q_PROPERTY(quint32 cardIndex READ cardIndex NOTIFY cardIndexChanged)
    Q_PROPERTY(QList<Port *> ports READ ports NOTIFY portsChanged)
    Q_PROPERTY(quint32 activePortIndex READ activePortIndex WRITE setActivePortIndex NOTIFY activePortIndexChanged)
    Q_PROPERTY(bool default READ isDefault WRITE setDefault NOTIFY defaultChanged)
    Q_PROPERTY(qint64 baseVolume READ baseVolume NOTIFY baseVolumeChanged)
    Q_PROPERTY(QVariantMap pulseProperties READ pulseProperties NOTIFY pulsePropertiesChanged)
    Q_PROPERTY(bool virtualDevice READ isVirtualDevice NOTIFY virtualDeviceChanged)

public:
    enum State {
        /** This state is used when the server does not support sink/source state introspection. */
        InvalidState = 0,
        /** Running, sink/source is playing/recording and used by at least one non-corked sink-input/source-output.  */
        RunningState,
        /** When idle, the sink/source is playing/recording but there is no non-corked sink-input/source-output attached to it. */
        IdleState,
        /** When suspended, actual sink/source access can be closed, for instance. */
        SuspendedState,
        UnknownState,
    };
    Q_ENUM(State);

    ~Device();

    /**
     * The state of this device.
     */
    State state() const;

    /**
     * A human readable description of this device.
     */
    QString description() const;

    /**
     * The device's form factor.
     * One of "internal", "speaker", "handset", "tv", "webcam", "microphone", "headset", "headphone", "hands-free", "car", "hifi", "computer", "portable".
     * This is based on PA_PROP_DEVICE_FORM_FACTOR.
     */
    QString formFactor() const;

    /**
     * Index of the card that owns this device.
     */
    quint32 cardIndex() const;

    /**
     * The ports associated with this device.
     */
    QList<Port *> ports() const;

    /**
     * The currently active port, by index.
     */
    quint32 activePortIndex() const;

    /**
     * Set the currently active port, by index.
     */
    virtual void setActivePortIndex(quint32 port_index) = 0;

    /**
     * Whether this is the default device.
     */
    virtual bool isDefault() const = 0;

    /**
     * Set whether this is the default device.
     */
    virtual void setDefault(bool enable) = 0;

    /**
     * The base volume of this device, generally useful as a good default volume.
     */
    qint64 baseVolume() const;

    /**
     * @return QVariantMap the pulseaudio properties of this device (e.g. media.class, device.api, ...)
     */
    [[nodiscard]] QVariantMap pulseProperties() const;

    /**
     * @brief Switch all streams onto this Device
     * Iterates through all relevant streams for the Device type and assigns them to this Device.
     * For example for a Sink device all SinkInputs known to the daemon will be explicitly switched
     * onto this Sink. Useful for mass-rerouting of streams from different devices onto a specific target device.
     */
    virtual Q_INVOKABLE void switchStreams() = 0;

    /**
     * @returns true when the device is a virtual device (e.g. a software-only sink)
     */
    bool isVirtualDevice() const;

Q_SIGNALS:
    void stateChanged();
    void descriptionChanged();
    void formFactorChanged();
    void cardIndexChanged();
    void portsChanged();
    void activePortIndexChanged();
    void defaultChanged();
    void baseVolumeChanged();
    void pulsePropertiesChanged();
    void virtualDeviceChanged();

protected:
    /** @private */
    explicit Device(QObject *parent);
    /** @private */
    DevicePrivate *d;

private:
    friend class SinkPrivate;
    friend class SourcePrivate;
};

} // PulseAudioQt

#endif // DEVICE_H
