/*
    SPDX-FileCopyrightText: 2017 David Rosca <nowrep@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "module.h"

#include "context.h"
#include "debug.h"
#include "indexedpulseobject_p.h"

namespace PulseAudioQt
{
Module::Module(QObject *parent)
    : IndexedPulseObject(parent)
{
}

void Module::update(const pa_module_info *info)
{
    IndexedPulseObject::d->updatePulseObject(info);
    PulseObject::d->updateProperties(info);

    const QString infoArgument = QString::fromUtf8(info->argument);
    if (m_argument != infoArgument) {
        m_argument = infoArgument;
        Q_EMIT argumentChanged();
    }
}

QString Module::name() const
{
    return m_name;
}

QString Module::argument() const
{
    return m_argument;
}

} // PulseAudioQt
