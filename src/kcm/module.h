/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef MODULE_H
#define MODULE_H

#include <KQuickAddons/ConfigModule>

class KCMPulseAudio : public KQuickAddons::ConfigModule
{
    Q_OBJECT
public:
    KCMPulseAudio(QObject *parent, const QVariantList &args);
    ~KCMPulseAudio() override;

public Q_SLOTS:
    void defaults() final;
    void load() final;
    void save() final;
};

#endif // MODULE_H
