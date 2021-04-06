/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "module.h"

#include <KAboutData>
#include <KLocalizedString>
#include <KPluginFactory>

#include "version.h"

K_PLUGIN_CLASS_WITH_JSON(KCMPulseAudio, "metadata.json")

KCMPulseAudio::KCMPulseAudio(QObject *parent, const QVariantList &args)
    : KQuickAddons::ConfigModule(parent, args)
{
    KAboutData *aboutData = new KAboutData("kcm_pulseaudio",
                                           i18nc("@title", "Audio"),
                                           QStringLiteral(PLASMAPA_VERSION_STRING),
                                           QStringLiteral(""),
                                           KAboutLicense::LicenseKey::GPL_V3,
                                           i18nc("@info:credit", "Copyright 2015 Harald Sitter"));

    aboutData->addAuthor(i18nc("@info:credit", "Harald Sitter"), i18nc("@info:credit", "Author"), QStringLiteral("apachelogger@kubuntu.org"));

    setAboutData(aboutData);
    setButtons(Help);
}

KCMPulseAudio::~KCMPulseAudio()
{
}

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
