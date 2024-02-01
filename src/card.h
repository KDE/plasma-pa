/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef CARD_H
#define CARD_H

#include "cardport.h"
#include "indexedpulseobject.h"
#include "profile.h"
#include "sink.h"
#include "source.h"

struct pa_card_info;

namespace PulseAudioQt
{
class CardPort;
class Profile;

class PULSEAUDIOQT_EXPORT Card : public IndexedPulseObject
{
    Q_OBJECT
    Q_PROPERTY(QList<Profile *> profiles READ profiles NOTIFY profilesChanged)
    Q_PROPERTY(quint32 activeProfileIndex READ activeProfileIndex WRITE setActiveProfileIndex NOTIFY activeProfileIndexChanged)
    Q_PROPERTY(QList<CardPort *> ports READ ports NOTIFY portsChanged)
    Q_PROPERTY(QList<Sink *> sinks READ sinks NOTIFY sinksChanged)
    Q_PROPERTY(QList<Source *> sources READ sources NOTIFY sourcesChanged)

public:
    ~Card();

    QList<Profile *> profiles() const;
    quint32 activeProfileIndex() const;
    void setActiveProfileIndex(quint32 profileIndex);
    QList<CardPort *> ports() const;
    QList<Sink *> sinks() const;
    QList<Source *> sources() const;

Q_SIGNALS:
    void profilesChanged();
    void activeProfileIndexChanged();
    void portsChanged();
    void sinksChanged();
    void sourcesChanged();

private:
    explicit Card(QObject *parent);

    class CardPrivate *const d;
    friend class MapBase<Card, pa_card_info>;
};

} // PulseAudioQt

#endif // CARD_H
