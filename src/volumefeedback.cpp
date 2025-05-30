/*
    SPDX-FileCopyrightText: 2008 Helio Chissini de Castro <helio@kde.org>
    SPDX-FileCopyrightText: 2016 David Rosca <nowrep@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "volumefeedback.h"
#include "canberracontext.h"
#include "soundthemeconfig.h"

VolumeFeedback::VolumeFeedback(QObject *parent)
    : QObject(parent)
    , m_config(new SoundThemeConfig(this))
{
    PulseAudioQt::CanberraContext::instance()->ref();
    if (ca_context_set_driver(PulseAudioQt::CanberraContext::instance()->canberra(), "pulse") != CA_SUCCESS) {
        return;
    }

    connect(m_config, &SoundThemeConfig::soundThemeChanged, this, &VolumeFeedback::updateCachedSound);
    updateCachedSound();
}

VolumeFeedback::~VolumeFeedback()
{
    PulseAudioQt::CanberraContext::instance()->unref();
}

bool VolumeFeedback::isValid() const
{
    return PulseAudioQt::CanberraContext::instance()->canberra();
}

void VolumeFeedback::play(quint32 sinkIndex)
{
    auto context = PulseAudioQt::CanberraContext::instance()->canberra();

    if (!context) {
        return;
    }

    int playing = 0;
    const int cindex = 2; // Note "2" is simply the index we've picked. It's somewhat irrelevant.
    ca_context_playing(context, cindex, &playing);

    // NB Depending on how this is desired to work, we may want to simply
    // skip playing, or cancel the currently playing sound and play our
    // new one... for now, let's do the latter.
    if (playing) {
        ca_context_cancel(context, cindex);
    }

    char dev[64];
    snprintf(dev, sizeof(dev), "%lu", (unsigned long)sinkIndex);
    ca_context_change_device(context, dev);

    // Ideally we'd use something like ca_gtk_play_for_widget()...
    /* clang-format off */
    ca_context_play(context, cindex,
                    CA_PROP_EVENT_ID, "audio-volume-change",
                    CA_PROP_CANBERRA_CACHE_CONTROL, "permanent", // For better performance
                    nullptr);
    /* clang-format on */

    ca_context_change_device(context, nullptr);
}

void VolumeFeedback::updateCachedSound()
{
    auto context = PulseAudioQt::CanberraContext::instance()->canberra();
    if (!context) {
        return;
    }

    /* clang-format off */
    ca_context_cache(context,
                     CA_PROP_EVENT_DESCRIPTION, "Volume Control Feedback Sound",
                     CA_PROP_EVENT_ID, "audio-volume-change",
                     // The default role "event" is played on the Notification sound channel
                     // which is muted in do not disturb mode. Volume change should always play.
                     CA_PROP_MEDIA_ROLE, "alert",
                     CA_PROP_CANBERRA_ENABLE, "1",
                     CA_PROP_CANBERRA_XDG_THEME_NAME, m_config->soundTheme().toLatin1().constData(),
                     nullptr);
    /* clang-format on */
}

#include "moc_volumefeedback.cpp"
