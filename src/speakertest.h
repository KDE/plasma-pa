/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>
    SPDX-FileCopyrightText: 2021 Nicolas Fella

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#pragma once

#include <PulseAudioQt/Sink>

#include <QObject>

class SpeakerTest : public QObject
{
    Q_OBJECT
    Q_PROPERTY(PulseAudioQt::Sink *sink READ sink WRITE setSink NOTIFY sinkChanged)
public:
    PulseAudioQt::Sink *sink() const;
    void setSink(PulseAudioQt::Sink *sink);
    Q_SIGNAL void sinkChanged();

    Q_INVOKABLE void testChannel(const QString &name);

private:
    PulseAudioQt::Sink *m_sink;
};
