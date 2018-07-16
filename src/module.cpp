/*
    Copyright 2017 David Rosca <nowrep@gmail.com>

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

#include "debug.h"

#include "context.h"

namespace QPulseAudio
{

Module::Module(QObject *parent)
    : PulseObject(parent)
{
}

void Module::update(const pa_module_info *info)
{
    updatePulseObject(info);

    const QString infoName = QString::fromUtf8(info->name);
    if (m_name != infoName) {
        m_name = infoName;
        Q_EMIT nameChanged();
    }
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

} // QPulseAudio
