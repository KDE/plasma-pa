/*
    SPDX-FileCopyrightText: 2017 David Rosca <nowrep@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "module.h"
#include "debug.h"
#include "module_p.h"

#include "context.h"
#include "indexedpulseobject_p.h"

namespace PulseAudioQt
{
Module::Module(QObject *parent)
    : IndexedPulseObject(parent)
    , d(new ModulePrivate(this))
{
}

ModulePrivate::ModulePrivate(Module *q)
    : q(q)
{
}

ModulePrivate::~ModulePrivate()
{
}

void ModulePrivate::update(const pa_module_info *info)
{
    q->IndexedPulseObject::d->updatePulseObject(info);
    q->PulseObject::d->updateProperties(info);

    const QString infoArgument = QString::fromUtf8(info->argument);
    if (m_argument != infoArgument) {
        m_argument = infoArgument;
        Q_EMIT q->argumentChanged();
    }
}

Module::~Module()
{
    delete d;
}

QString Module::argument() const
{
    return d->m_argument;
}

} // PulseAudioQt
