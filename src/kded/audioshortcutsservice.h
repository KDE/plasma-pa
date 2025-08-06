/*
    SPDX-FileCopyrightText: 2023 Bharadwaj Raju <bharadwaj.raju777@gmail.com>
    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#pragma once

#include <memory>

#include <KConfigWatcher>
#include <kdedmodule.h>

#include <PulseAudioQt/Context>
#include <PulseAudioQt/Models>

#include "globalconfig.h"
#include "osdservice.h"
#include "preferreddevice.h"
#include "volumefeedback.h"

class MutedMicrophoneReminder;

namespace PulseAudioQt
{
class Device;
} // namespace PulseAudioQt

using namespace Qt::Literals::StringLiterals;

// DEFAULT_SINK_NAME in module-always-sink.c
constexpr QLatin1String DUMMY_OUTPUT_NAME = "auto_null"_L1;

class AudioShortcutsService : public KDEDModule
{
    Q_OBJECT
public:
    AudioShortcutsService(QObject *parent, const QList<QVariant> &);

    static QString nameForDevice(const PulseAudioQt::Device *device);

private:
    static qint64 boundVolume(qint64 volume, int maxVolume);
    static int volumePercent(qint64 volume);
    int changeVolumePercent(PulseAudioQt::Device *device, int deltaPercent);
    void handleDefaultSinkChange();
    void muteVolume();
    void playFeedback(int sinkIdx);
    void showMute(int percent);
    void showVolume(int percent);
    void showMicMute(int percent);
    void showMicVolume(int percent);

    void applyGlobalSinkMute(const QModelIndex &index, const bool globalMute, QStringList &globalMuteMutedDevices);
    void applyGlobalSourceMute(const QModelIndex &index, const bool globalMute, QStringList &globalMuteMutedDevices);
    void handleNewSink(const QModelIndex &parent, int first, int last);
    void handleNewSource(const QModelIndex &parent, int first, int last);
    void handleSinkDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QList<int> &roles);
    void handleSourceDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QList<int> &roles);
    void enableGlobalSinkMute();
    void enableGlobalSourceMute();
    void disableGlobalSinkMute();
    void disableGlobalSourceMute();

    PulseAudioQt::SinkModel *m_sinkModel = nullptr;
    PulseAudioQt::SourceModel *m_sourceModel = nullptr;
    PulseAudioQt::CardModel *m_cardModel = nullptr;
    GlobalConfig *m_globalConfig;
    KConfigWatcher::Ptr m_globalConfigWatcher;
    OsdServiceInterface *m_osdDBusInterface;
    VolumeFeedback *m_feedback;
    bool m_hasDefaultSink = false;
    PreferredDevice m_preferredDevice;
    std::unique_ptr<MutedMicrophoneReminder> m_mutedMicrophoneReminder;
};
