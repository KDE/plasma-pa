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

#ifndef SOURCEOUTPUT_H
#define SOURCEOUTPUT_H

#include "stream.h"

namespace QPulseAudio
{

class Q_DECL_EXPORT SourceOutput : public Stream
{
    Q_OBJECT
public:
    SourceOutput(QObject *parent);

    void update(const pa_source_output_info *info);

    void setVolume(qint64 volume) Q_DECL_OVERRIDE;
    void setMuted(bool muted) Q_DECL_OVERRIDE;
    void setChannelVolume(int channel, qint64 volume) Q_DECL_OVERRIDE;
    void setDeviceIndex(quint32 deviceIndex) Q_DECL_OVERRIDE;
};

} // QPulseAudio

#endif // SOURCEOUTPUT_H
