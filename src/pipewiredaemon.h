// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Harald Sitter <sitter@kde.org>

#pragma once

#include <QVersionNumber>

#include <PulseAudioQt/Client>

class PipeWireDaemon : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool hasBuggedConfigMerging READ hasBuggedConfigMerging NOTIFY versionChanged)
public:
    explicit PipeWireDaemon(QObject *parent = nullptr);
    [[nodiscard]] bool hasBuggedConfigMerging() const;

Q_SIGNALS:
    void versionChanged();

private:
    void checkVersion(PulseAudioQt::Client *client);
    std::optional<QString> m_version;
    QMetaObject::Connection m_versionDetectionConnection;
};
