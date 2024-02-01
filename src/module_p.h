/*
    SPDX-FileCopyrightText: 2018 Nicolas Fella <nicolas.fella@gmx.de>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/
#ifndef MODULE_P_H
#define MODULE_P_H

#include "module.h"
#include <pulse/introspect.h>

namespace PulseAudioQt
{
class ModulePrivate
{
public:
    explicit ModulePrivate(Module *q);
    virtual ~ModulePrivate();

    void update(const pa_module_info *info);

    Module *q;

    QString m_argument;
};

} // PulseAudioQt

#endif
