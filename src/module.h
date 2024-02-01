/*
    SPDX-FileCopyrightText: 2017 David Rosca <nowrep@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef MODULE_H
#define MODULE_H

#include <pulse/introspect.h>

#include <QMap>
#include <QVariant>

#include "indexedpulseobject.h"

namespace PulseAudioQt
{
class Module : public IndexedPulseObject
{
    Q_OBJECT
    Q_PROPERTY(QString argument READ argument NOTIFY argumentChanged)

public:
    explicit Module(QObject *parent);

    void update(const pa_module_info *info);

    QString name() const;
    QString argument() const;

Q_SIGNALS:
    void nameChanged();
    void argumentChanged();

private:
    QString m_name;
    QString m_argument;
};

} // PulseAudioQt

#endif // MODULE_H
