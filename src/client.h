/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef CLIENT_H
#define CLIENT_H

#include <pulse/introspect.h>

#include <QMap>

#include "indexedpulseobject.h"

namespace PulseAudioQt
{
class Client : public IndexedPulseObject
{
    Q_OBJECT
public:
    explicit Client(QObject *parent);
    ~Client() override;

    void update(const pa_client_info *info);

    QString name() const;

Q_SIGNALS:
    void nameChanged();

private:
    QString m_name;
};

} // PulseAudioQt

#endif // CLIENT_H
