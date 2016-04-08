/*
    Copyright 2014-2015 Harald Sitter <sitter@kde.org>

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

#include "module.h"

#include <KAboutData>
#include <KDeclarative/KDeclarative>
#include <KLocalizedString>
#include <KPluginFactory>

#include <context.h>
#include <pulseaudio.h>

#include "version.h"

K_PLUGIN_FACTORY_WITH_JSON(KCMPulseAudioFactory,
                           "kcm_pulseaudio.json",
                           registerPlugin<KCMPulseAudio>();)

KCMPulseAudio::KCMPulseAudio(QObject *parent, const QVariantList &args)
    : KQuickAddons::ConfigModule(parent, args)
{
    KAboutData *aboutData = new KAboutData("kcm_pulseaudio",
                                           i18nc("@title", "PulseAudio"),
                                           global_s_versionStringFull,
                                           QStringLiteral(""),
                                           KAboutLicense::LicenseKey::GPL_V3,
                                           i18nc("@info:credit", "Copyright 2015 Harald Sitter"));

    aboutData->addAuthor(i18nc("@info:credit", "Harald Sitter"),
                        i18nc("@info:credit", "Author"),
                        QStringLiteral("apachelogger@kubuntu.org"));

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
