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

#include <KCModule>

namespace Ui {
    class Module;
}

class Context;

class Module : public KCModule
{
    Q_OBJECT
public:
    /**
     * Constructor.
     *
     * @param parent Parent widget of the module
     * @param args Arguments for the module
     */
    explicit Module(QWidget *parent, const QVariantList &args = QVariantList());

    /*** Destructor. */
    ~Module();

    /*** Overloading the KCModule load() function. */
    void load() Q_DECL_OVERRIDE Q_DECL_FINAL;

    /*** Overloading the KCModule save() function. */
    void save() Q_DECL_OVERRIDE Q_DECL_FINAL;

    /*** Overloading the KCModule defaults() function. */
    void defaults() Q_DECL_OVERRIDE Q_DECL_FINAL;

private:
    /*** UI */
    Ui::Module *ui;

    Context *m_context;
};

#endif // MODULE_H
