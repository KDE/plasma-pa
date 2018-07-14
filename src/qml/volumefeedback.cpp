/*
    Copyright 2008 Helio Chissini de Castro <helio@kde.org>
    Copyright 2016 David Rosca <nowrep@gmail.com>

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

#include "volumefeedback.h"
#include "canberracontext.h"

VolumeFeedback::VolumeFeedback(QObject *parent)
    : QObject(parent)
{
    QPulseAudio::CanberraContext::instance()->ref();
    if (ca_context_set_driver(QPulseAudio::CanberraContext::instance()->canberra(), "pulse") < 0) {
        return;
    }
}

VolumeFeedback::~VolumeFeedback()
{
    QPulseAudio::CanberraContext::instance()->unref();
}

bool VolumeFeedback::isValid() const
{
    return QPulseAudio::CanberraContext::instance()->canberra();
}

void VolumeFeedback::play(quint32 sinkIndex)
{
    auto context = QPulseAudio::CanberraContext::instance()->canberra();

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
    snprintf(dev, sizeof(dev), "%lu", (unsigned long) sinkIndex);
    ca_context_change_device(context, dev);

    // Ideally we'd use something like ca_gtk_play_for_widget()...
    ca_context_play(
        context,
        cindex,
        CA_PROP_EVENT_DESCRIPTION, "Volume Control Feedback Sound",
        CA_PROP_EVENT_ID, "audio-volume-change",
        CA_PROP_CANBERRA_CACHE_CONTROL, "permanent",
        CA_PROP_CANBERRA_ENABLE, "1",
        nullptr
    );

    ca_context_change_device(context, nullptr);
}
