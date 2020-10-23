/*
    Copyright 2018 Nicolas Fella <nicolas.fella@gmx.de>

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

#include "canberracontext.h"

namespace QPulseAudio
{

CanberraContext *CanberraContext::s_context = nullptr;

CanberraContext *CanberraContext::instance()
{
    if (!s_context) {
        s_context = new CanberraContext;
    }
    return s_context;
}

CanberraContext::CanberraContext(QObject *parent)
    : QObject(parent)
{
    ca_context_create(&m_canberra);
}


CanberraContext::~CanberraContext()
{
    if (m_canberra) {
        ca_context_destroy(m_canberra);
    }
}

ca_context *CanberraContext::canberra()
{
    return m_canberra;
}

void CanberraContext::ref()
{
    ++m_references;
}

void CanberraContext::unref()
{
    if (--m_references == 0) {
        delete this;
        s_context = nullptr;
    }
}
}
