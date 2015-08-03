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

#ifndef PORT_H
#define PORT_H

#include "profile.h"

namespace QPulseAudio
{

class Q_DECL_EXPORT Port : public Profile
{
    Q_OBJECT
    Q_PROPERTY(bool available READ isAvailable NOTIFY availableChanged)
public:
    Port(QObject *parent);
    virtual ~Port();

    template<typename PAInfo>
    void setInfo(const PAInfo *info)
    {
        Profile::setInfo(info);
        if (m_isAvailable != info->available) {
            m_isAvailable = info->available;
            emit availableChanged();
        }
    }

    bool isAvailable() const;

signals:
    void availableChanged();

private:
    bool m_isAvailable;
};

} // QPulseAudio

#endif // PORT_H
