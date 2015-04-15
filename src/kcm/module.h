/*
  Copyright (C) 2012 Harald Sitter <apachelogger@ubuntu.com>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License or (at your option) version 3 or any later version
  accepted by the membership of KDE e.V. (or its successor approved
  by the membership of KDE e.V.), which shall act as a proxy
  defined in Section 14 of version 3 of the license.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MODULE_H
#define MODULE_H

#include <KQuickAddons/ConfigModule>

class CardModel;
class Context;
class SinkInputModel;
class SinkModel;
class SourceModel;
class SourceOutputModel;

class KCMPulseAudio : public KQuickAddons::ConfigModule
{
    Q_OBJECT
public:
    KCMPulseAudio(QObject *parent, const QVariantList &args);
    ~KCMPulseAudio();

public slots:
    void defaults() Q_DECL_OVERRIDE Q_DECL_FINAL;
    void load() Q_DECL_OVERRIDE Q_DECL_FINAL;
    void save() Q_DECL_OVERRIDE Q_DECL_FINAL;

private:
    Context *m_context;
};

#endif // MODULE_H
