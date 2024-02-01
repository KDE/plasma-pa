/*
    SPDX-FileCopyrightText: 2021 Nicolas Fella <nicolas.fella@gmx.de>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef INDEXEDPULSEOBJECT_H
#define INDEXEDPULSEOBJECT_H

#include <QObject>

#include "pulseaudioqt_export.h"
#include "pulseobject.h"

namespace PulseAudioQt
{
class PULSEAUDIOQT_EXPORT IndexedPulseObject : public PulseObject
{
    Q_OBJECT
    Q_PROPERTY(quint32 index READ index CONSTANT)

public:
    ~IndexedPulseObject();

    /**
     * Index of this object.
     */
    quint32 index() const;

protected:
    /** @private */
    explicit IndexedPulseObject(QObject *parent);
    /** @private */
    class IndexedPulseObjectPrivate *const d;

private:
    // Ensure that we get properly parented.
    IndexedPulseObject();
    friend class ClientPrivate;
    friend class CardPrivate;
    friend class ModulePrivate;
    friend class VolumeObjectPrivate;
    friend class ProfilePrivate;
};

} // PulseAudioQt

#endif // INDEXEDPULSEOBJECT_H
