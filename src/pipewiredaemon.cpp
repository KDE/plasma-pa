// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Harald Sitter <sitter@kde.org>

#include "pipewiredaemon.h"

#include <QVariantMap>

#include <PulseAudioQt/Context>

using namespace Qt::StringLiterals;

PipeWireDaemon::PipeWireDaemon(QObject *parent)
    : QObject(parent)
    , m_versionDetectionConnection(connect(PulseAudioQt::Context::instance(), &PulseAudioQt::Context::clientAdded, this, &PipeWireDaemon::checkVersion))
{
    for (const auto &client : PulseAudioQt::Context::instance()->clients()) {
        checkVersion(client);
    }
}

bool PipeWireDaemon::hasBuggedConfigMerging() const
{
    // https://gitlab.freedesktop.org/pipewire/pipewire/-/issues/4428
    // Assume everything is OK. If it isn't we'll know once client data arrives.
    auto ret = QVersionNumber::fromString(m_version.value_or(u"1.3.0"_s)) < QVersionNumber(1, 3, 0);
    return ret;
}

void PipeWireDaemon::checkVersion(PulseAudioQt::Client *client)
{
    if (client->name() != "pipewire"_L1) {
        return;
    }

    const auto version = client->properties().value(u"core.version"_s, QVariant()).toString();
    if (version.isEmpty()) {
        return;
    }

    // Once we know the version we can stop listening for new clients.
    disconnect(m_versionDetectionConnection);
    m_version = version;
    Q_EMIT versionChanged();
}
