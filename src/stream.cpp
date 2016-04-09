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

#include "stream.h"

namespace QPulseAudio
{

Stream::Stream(QObject *parent)
    : VolumeObject(parent)
    , m_clientIndex(PA_INVALID_INDEX)
    , m_virtualStream(false)
{
    m_volumeWritable = false;
    m_hasVolume = false;
}

Stream::~Stream()
{
}

QString Stream::name() const
{
    return m_name;
}

Client *Stream::client() const
{
    return context()->clients().data().value(m_clientIndex, nullptr);
}

bool Stream::isVirtualStream() const
{
    return m_virtualStream;
}

quint32 Stream::deviceIndex() const
{
    return m_deviceIndex;
}

} // QPulseAudio
