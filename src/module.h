/*
    SPDX-FileCopyrightText: 2017 David Rosca <nowrep@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef MODULE_H
#define MODULE_H

#include "indexedpulseobject.h"
#include "pulseaudioqt_export.h"

struct pa_module_info;

namespace PulseAudioQt
{
class PULSEAUDIOQT_EXPORT Module : public IndexedPulseObject
{
    Q_OBJECT
    Q_PROPERTY(QString argument READ argument NOTIFY argumentChanged)

public:
    ~Module();

    QString argument() const;

Q_SIGNALS:
    void argumentChanged();

private:
    explicit Module(QObject *parent);

    class ModulePrivate *const d;
    friend class MapBase<Module, pa_module_info>;
};

} // PulseAudioQt

#endif // MODULE_H
