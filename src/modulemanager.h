/*
    SPDX-FileCopyrightText: 2016 David Edmundson <davidedmundson@kde.org>
    SPDX-FileCopyrightText: 2024 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef MODULEMANAGER_H
#define MODULEMANAGER_H

#include <QObject>

namespace PulseAudioQt
{
class GenericModule;

class ModuleManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool combineSinks READ combineSinks WRITE setCombineSinks NOTIFY combineSinksChanged)
    Q_PROPERTY(bool switchOnConnect READ switchOnConnect WRITE setSwitchOnConnect NOTIFY switchOnConnectChanged)
public:
    explicit ModuleManager(QObject *parent = nullptr);
    ~ModuleManager() override;
    Q_DISABLE_COPY_MOVE(ModuleManager)

    bool combineSinks() const;
    void setCombineSinks(bool combineSinks);
    bool switchOnConnect() const;
    void setSwitchOnConnect(bool switchOnConnect);

Q_SIGNALS:
    void combineSinksChanged();
    void switchOnConnectChanged();

private:
    GenericModule *m_combineSinks;
    GenericModule *m_switchOnConnect;
};

} // PulseAudioQt

#endif // STREAM_H
