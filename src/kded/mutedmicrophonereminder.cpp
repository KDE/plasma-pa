/*
 * SPDX-FileCopyrightText: 2025 Kai Uwe Broulik <kde@broulik.de>
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "mutedmicrophonereminder.h"

#include "audioshortcutsservice.h"

#include <PulseAudioQt/Device>
#include <PulseAudioQt/Models>

#include <KLocalizedString>

#include <QString>

using namespace Qt::StringLiterals;

constexpr QLatin1String OSD_DBUS_SERVICE = "org.kde.plasmashell"_L1;
constexpr QLatin1String OSD_DBUS_PATH = "/org/kde/osdService"_L1;

MutedMicrophoneReminder::MutedMicrophoneReminder(QObject *parent)
    : QObject(parent)
    , m_sourceModel(new PulseAudioQt::SourceModel(this))
    , m_sourceOutputModel(new PulseAudioQt::SourceOutputModel(this))
    , m_osdDBusInterface(OSD_DBUS_SERVICE, OSD_DBUS_PATH, QDBusConnection::sessionBus())
{
    connect(m_sourceModel, &PulseAudioQt::SourceModel::rowsAboutToBeRemoved, this, &MutedMicrophoneReminder::onSourceAboutToBeRemoved);
    connect(m_sourceModel, &PulseAudioQt::SourceModel::dataChanged, this, &MutedMicrophoneReminder::onSourceDataChanged);

    connect(m_sourceOutputModel, &PulseAudioQt::SourceOutputModel::rowsInserted, this, &MutedMicrophoneReminder::onSourceOutputInserted);
    connect(m_sourceOutputModel, &PulseAudioQt::SourceOutputModel::dataChanged, this, &MutedMicrophoneReminder::onSourceOutputDataChanged);
}

void MutedMicrophoneReminder::onSourceAboutToBeRemoved(const QModelIndex &parent, int first, int last)
{
    for (int i = first; i <= last; ++i) {
        const QPersistentModelIndex sourceIndex = m_sourceModel->index(i, 0, parent);
        m_mutedSources.removeOne(sourceIndex);
    }
}

void MutedMicrophoneReminder::onSourceDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QList<int> &roles)
{
    static int s_sourceMutedRole = m_sourceModel->role("Muted");
    Q_ASSERT(s_sourceMutedRole > -1);

    if (roles.isEmpty() || roles.contains(s_sourceMutedRole)) {
        for (int i = topLeft.row(); i <= bottomRight.row(); ++i) {
            const QModelIndex index = topLeft.siblingAtRow(i);
            const bool muted = index.data(s_sourceMutedRole).toBool();
            if (!muted) {
                m_mutedSources.removeOne(QPersistentModelIndex(index));
            }
        }
    }
}

void MutedMicrophoneReminder::onSourceOutputInserted(const QModelIndex &parent, int first, int last)
{
    for (int i = first; i <= last; ++i) {
        const QModelIndex index = m_sourceOutputModel->index(i, 0, parent);
        notifySourceOutput(index);
    }
}

void MutedMicrophoneReminder::onSourceOutputDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QList<int> &roles)
{
    static int s_sourceOutputCorkedRole = m_sourceOutputModel->role("Corked");
    Q_ASSERT(s_sourceOutputCorkedRole > -1);

    if (roles.isEmpty() || roles.contains(s_sourceOutputCorkedRole)) {
        for (int i = topLeft.row(); i <= bottomRight.row(); ++i) {
            const QModelIndex index = topLeft.siblingAtRow(i);
            notifySourceOutput(index);
        }
    }
}

void MutedMicrophoneReminder::notifySourceOutput(const QModelIndex &sourceOutputIndex)
{
    static int s_sourceOutputCorkedRole = m_sourceOutputModel->role("Corked");
    static int s_sourceOutputDeviceIndexRole = m_sourceOutputModel->role("DeviceIndex");
    static int s_sourceMutedRole = m_sourceModel->role("Muted");
    static int s_sourceIndexRole = m_sourceModel->role("Index");
    Q_ASSERT(s_sourceOutputCorkedRole > -1);
    Q_ASSERT(s_sourceOutputDeviceIndexRole > -1);
    Q_ASSERT(s_sourceMutedRole > -1);
    Q_ASSERT(s_sourceIndexRole > -1);

    const bool corked = sourceOutputIndex.data(s_sourceOutputCorkedRole).toBool();
    if (corked) {
        return;
    }

    // Find corresponding device.
    const auto deviceIndex = sourceOutputIndex.data(s_sourceOutputDeviceIndexRole).toUInt();

    QPersistentModelIndex persistentSourceIndex;
    for (int i = 0; i < m_sourceModel->rowCount(); ++i) {
        const QModelIndex sourceIndex = m_sourceModel->index(i);

        const auto index = sourceIndex.data(s_sourceIndexRole).toUInt();
        if (index == deviceIndex) {
            persistentSourceIndex = sourceIndex;
            break;
        }
    }

    if (!persistentSourceIndex.isValid()) {
        return;
    }

    // Not muted, don't show anything.
    const bool muted = persistentSourceIndex.data(s_sourceMutedRole).toBool();
    if (!muted) {
        m_mutedSources.removeOne(persistentSourceIndex);
        return;
    }

    // Was already muted, don't show anything.
    if (m_mutedSources.contains(persistentSourceIndex)) {
        return;
    }

    m_mutedSources.append(persistentSourceIndex);

    if (m_sourceModel->rowCount() > 1) {
        auto *device = persistentSourceIndex.data(PulseAudioQt::AbstractModel::PulseObjectRole).value<PulseAudioQt::Device *>();
        m_osdDBusInterface.showText(u"microphone-sensitivity-muted"_s,
                                    i18nc("Microphone name: is currently muted (osd, keep short)", "%1: Muted", AudioShortcutsService::nameForDevice(device)));
    } else {
        m_osdDBusInterface.microphoneVolumeChanged(0);
    }
}

#include "moc_mutedmicrophonereminder.cpp"
