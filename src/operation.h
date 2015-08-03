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

#ifndef OPERATION_H
#define OPERATION_H

#include <pulse/operation.h>

namespace QPulseAudio
{

/**
 * @brief The PAOperation class
 * Helps with management of pa_operations. pa_operations need to be expicitly
 * unref'd after use, so this class is essentially a fancy scoping helper where
 * destruction of an instance would also unref the held operation (if there is
 * one).
 */
class PAOperation
{
public:
    /**
     * @brief PAOperation
     * @param operation operation to manage the scope of
     */
    PAOperation(pa_operation *operation = nullptr);
    ~PAOperation();

    PAOperation &operator =(pa_operation *operation);

    /**
     * @brief operator !
     * @return whether or not there is an operation pointer
     */
    bool operator !();

    /**
     * @brief operator bool representing whether there is an operation
     */
    operator bool();

    /**
     * @brief operator *
     * @return pointer to internal pa_operation object
     */
    pa_operation *&operator *();

private:
    pa_operation *m_operation;
};

} // QPulseAudio

#endif // OPERATION_H
