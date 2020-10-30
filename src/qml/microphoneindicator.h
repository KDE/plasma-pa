/*
    Copyright 2019 Kai Uwe Broulik <kde@privat.broulik.de>
    Copyright 2020 MBition GmbH,
        Author: Kai Uwe Broulik <kai_uwe.broulik@mbition.io>

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
#include <QPersistentModelIndex>
#include <QPointer>
#include <QVector>

class QAction;
class QTimer;

class KStatusNotifierItem;

class VolumeOSD;

namespace QPulseAudio
{
class Source;
class SourceModel;
class SourceOutputModel;
}

class MicrophoneIndicator : public QObject
{
    Q_OBJECT

public:
    explicit MicrophoneIndicator(QObject *parent = nullptr);
    ~MicrophoneIndicator() override;

    Q_INVOKABLE void init();

Q_SIGNALS:
    void enabledChanged();

private:
    void scheduleUpdate();
    void update();

    bool muted() const;
    void setMuted(bool muted);
    void toggleMuted();

    void adjustVolume(int direction);

    static int volumePercent(QPulseAudio::Source *source);
    void showOsd();

    QVector<QModelIndex> recordingApplications() const;
    QString toolTipForApps(const QVector<QModelIndex> &apps) const;
    QString sourceOutputDisplayName(const QModelIndex &idx) const;

    QPulseAudio::SourceModel *m_sourceModel = nullptr; // microphone devices
    QPulseAudio::SourceOutputModel *m_sourceOutputModel = nullptr; // recording streams

    KStatusNotifierItem *m_sni = nullptr;
    QPointer<QAction> m_muteAction;
    QPointer<QAction> m_dontAgainAction;

    QVector<QPersistentModelIndex> m_mutedIndices;

    VolumeOSD *m_osd = nullptr;
    bool m_showOsdOnUpdate = false;

    int m_wheelDelta = 0;

    QTimer *m_updateTimer;

};
