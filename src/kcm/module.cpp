/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "module.h"

#include <KPluginFactory>

K_PLUGIN_CLASS_WITH_JSON(KCMPulseAudio, "kcm_pulseaudio.json")

KCMPulseAudio::KCMPulseAudio(QObject *parent, const KPluginMetaData &metaData, const QVariantList &args)
    : KQuickAddons::ConfigModule(parent, metaData, args)
{
    setButtons(Help);
}

KCMPulseAudio::~KCMPulseAudio() = default;

void KCMPulseAudio::load()
{
}

void KCMPulseAudio::save()
{
}

void KCMPulseAudio::defaults()
{
}

#include "module.moc"
