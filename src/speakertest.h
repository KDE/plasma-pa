/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>
    SPDX-FileCopyrightText: 2021 Nicolas Fella

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#pragma once

#include "sink.h"

#include <QObject>

class SpeakerTest : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QPulseAudio::Sink *sink READ sink WRITE setSink NOTIFY sinkChanged)
    Q_PROPERTY(QStringList playingChannels READ playingChannels NOTIFY playingChannelsChanged)
public:
    QPulseAudio::Sink *sink() const;
    void setSink(QPulseAudio::Sink *sink);
    Q_SIGNAL void sinkChanged();

    QStringList playingChannels() const;
    Q_SIGNAL void playingChannelsChanged();
    void playingFinished(const QString &name, int errorCode);

    Q_INVOKABLE void testChannel(const QString &name);
    Q_SIGNAL void showErrorMessage(const QString &message);

private:
    QPulseAudio::Sink *m_sink;
    QStringList m_playingChannels;
};
