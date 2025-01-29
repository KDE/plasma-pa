/*
 * SPDX-FileCopyrightText: 2025 Kai Uwe Broulik <kde@broulik.de>
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <QPersistentModelIndex>
#include <QVector>

#include "osdservice.h"

namespace PulseAudioQt
{
class SourceModel;
class SourceOutputModel;
} // namespace PulseAudioQt

class MutedMicrophoneReminder : public QObject
{
    Q_OBJECT

public:
    explicit MutedMicrophoneReminder(QObject *parent = nullptr);

private:
    void onSourceAboutToBeRemoved(const QModelIndex &parent, int first, int last);
    void onSourceDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QList<int> &roles);

    void onSourceOutputInserted(const QModelIndex &parent, int first, int last);
    void onSourceOutputDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QList<int> &roles);

    void notifySourceOutput(const QModelIndex &sourceOutputIndex);

    PulseAudioQt::SourceModel *m_sourceModel = nullptr;
    PulseAudioQt::SourceOutputModel *m_sourceOutputModel = nullptr;
    OsdServiceInterface m_osdDBusInterface;
    QVector<QPersistentModelIndex> m_mutedSources;
};
