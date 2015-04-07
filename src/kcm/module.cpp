/*
  Copyright (C) 2012-2014 Harald Sitter <apachelogger@ubuntu.com>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License or (at your option) version 3 or any later version
  accepted by the membership of KDE e.V. (or its successor approved
  by the membership of KDE e.V.), which shall act as a proxy
  defined in Section 14 of version 3 of the license.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "module.h"
#include "ui_module.h"

#include <KAboutData>
#include <KColorScheme>
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
    , m_context(new Context(this))
{
    qmlRegisterType<CardModel>();
    qmlRegisterType<Context>();
    qmlRegisterType<SinkModel>();
    qmlRegisterType<SinkInputModel>();
    qmlRegisterType<SourceModel>();
    qmlRegisterType<SourceOutputModel>();

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
    setButtons(buttons() ^ Help ^ Default ^ Apply);
}

KCMPulseAudio::~KCMPulseAudio()
{
}

Context *KCMPulseAudio::context() const
{
    return m_context;
}

CardModel *KCMPulseAudio::cardModel()
{
    static CardModel *single = new CardModel(m_context, this);
    return single;
}

SinkModel *KCMPulseAudio::sinkModel()
{
    static SinkModel *single = new SinkModel(m_context, this);
    return single;
}

SinkInputModel *KCMPulseAudio::sinkInputModel()
{
    static SinkInputModel *single = new SinkInputModel(m_context, this);
    return single;
}

SourceModel *KCMPulseAudio::sourceModel()
{
    static SourceModel *single = new SourceModel(m_context, this);
    return single;
}

SourceOutputModel *KCMPulseAudio::sourceOutputModel()
{
    static SourceOutputModel *single = new SourceOutputModel(m_context, this);
    return single;
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
