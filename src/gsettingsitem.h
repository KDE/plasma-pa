/*
 * Copyright (C) 2018 Nicolas Fella <nicolas.fella@gmx.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#ifndef GSETTINGSITEM_H
#define GSETTINGSITEM_H

#include <QVariant>
#include <QStringList>
#include <QObject>

#include <gio/gio.h>

class GSettingsItem : public QObject
{
    Q_OBJECT

 public:

    explicit GSettingsItem(const QString &key, QObject *parent = nullptr);
    virtual ~GSettingsItem() override;

    QVariant value(const QString &key) const;
    void set(const QString &key, const QVariant &val);

    bool isValid() const;

Q_SIGNALS:
    void subtreeChanged();

private:
    GSettings *m_settings = nullptr;

static void settingChanged(GSettings *settings, const gchar *key, gpointer data)
{
    Q_UNUSED(settings)
    Q_UNUSED(key)

    GSettingsItem *self = static_cast<GSettingsItem *>(data);
    Q_EMIT self->subtreeChanged();
}

};

#endif // GCONFITEM_H
