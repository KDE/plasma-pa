/*
    SPDX-FileCopyrightText: 2018 Nicolas Fella <nicolas.fella@gmx.de>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/
#ifndef PULSEOBJECT_P_H
#define PULSEOBJECT_P_H

#include "debug.h"

#include <QVariantMap>

#include <pulse/introspect.h>

#include "context.h"

namespace PulseAudioQt
{
class PulseObjectPrivate
{
public:
    explicit PulseObjectPrivate(PulseObject *q);
    virtual ~PulseObjectPrivate();

    PulseObject *q;
    QVariantMap m_properties;
    QString m_name;

    template<typename PAInfo>
    void updatePulseObject(PAInfo *info)
    {
        if (m_name != QString::fromUtf8(info->name)) {
            m_name = QString::fromUtf8(info->name);
            Q_EMIT q->nameChanged();
        }
    }

    template<typename PAInfo>
    void updateProperties(PAInfo *info)
    {
        m_properties.clear();
        void *it = nullptr;
        while (const char *key = pa_proplist_iterate(info->proplist, &it)) {
            Q_ASSERT(key);
            const char *value = pa_proplist_gets(info->proplist, key);
            if (!value) {
                qCDebug(PULSEAUDIOQT) << "property" << key << "not a string";
                continue;
            }
            Q_ASSERT(value);
            m_properties.insert(QString::fromUtf8(key), QString::fromUtf8(value));
        }
        Q_EMIT q->propertiesChanged();
    }
};
}
#endif
