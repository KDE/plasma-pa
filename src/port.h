/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef PORT_H
#define PORT_H

#include "profile.h"
#include "pulseaudioqt_export.h"

namespace PulseAudioQt
{
/**
 * A PulseAudio port.
 */
class PULSEAUDIOQT_EXPORT Port : public Profile
{
    Q_OBJECT
    Q_PROPERTY(Type type READ type NOTIFY typeChanged)

public:
    ~Port();

    enum Type {
        Unknown,
        AUX,
        Speaker,
        Headphones,
        Line,
        Mic,
        Headset,
        Handset,
        Earpiece,
        SPDIF,
        HDMI,
        TV,
        Radio,
        Video,
        USB,
        Bluetooth,
        Portable,
        Handsfree,
        Car,
        HiFi,
        Phone,
        Network,
        Analog,
    };
    Q_ENUM(Type)

    Type type() const;

Q_SIGNALS:
    void typeChanged();

protected:
    /** @private */
    explicit Port(QObject *parent);
    /** @private */
    class PortPrivate *const d;

    friend class DevicePrivate;
    friend class CardPrivate;
};

} // PulseAudioQt

#endif // PORT_H
