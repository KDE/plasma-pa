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

#ifndef MODULE_H
#define MODULE_H

#include <pulse/introspect.h>

#include <QMap>
#include <QVariant>

#include "pulseobject.h"

namespace QPulseAudio
{

class Q_DECL_EXPORT Module : public PulseObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString argument READ argument NOTIFY argumentChanged)

public:
    Module(QObject *parent);

    void update(const pa_module_info *info);

    QString name() const;
    QString argument() const;

signals:
    void nameChanged();
    void argumentChanged();

private:
    QString m_name;
    QString m_argument;
};

} // QPulseAudio

#endif // MODULE_H
