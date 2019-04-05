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

#include <QString>
#include <QVariant>

#include <gio/gio.h>

#include "gsettingsitem.h"
#include "debug.h"

QVariant GSettingsItem::value(const QString &key) const
{
    GVariant *gvalue = g_settings_get_value(m_settings, key.toLatin1().data());

    QVariant toReturn;

    switch (g_variant_classify(gvalue)) {
    case G_VARIANT_CLASS_BOOLEAN:
        toReturn = QVariant((bool)g_variant_get_boolean(gvalue));
        break;
    case G_VARIANT_CLASS_STRING:
        toReturn = QVariant(QString::fromUtf8(g_variant_get_string(gvalue, nullptr)));
        break;
    default:
        qCWarning(PLASMAPA()) << "Unhandled variant type in value()";
    }

    g_variant_unref(gvalue);

    return toReturn;
}

void GSettingsItem::set(const QString &key, const QVariant &val)
{
    // It might be hard to detect the right GVariant type from
    // complext QVariant types such as string lists or more detailed
    // types such as integers (GVariant has different sizes),
    // therefore we get the current value for the key and convert
    // to QVariant using the GVariant type
    GVariant *oldValue = g_settings_get_value(m_settings, key.toLatin1().data());
    GVariant *newValue = nullptr;

    switch (g_variant_type_peek_string(g_variant_get_type(oldValue))[0]) {
    case G_VARIANT_CLASS_BOOLEAN:
        newValue = g_variant_new_boolean(val.toBool());
        break;
    case G_VARIANT_CLASS_STRING:
        newValue = g_variant_new_string(val.toString().toUtf8().constData());
        break;
    default:
        qCWarning(PLASMAPA()) << "Unhandled variant type in set()";
    }

    if (newValue) {
        g_settings_set_value(m_settings, key.toLatin1().data(), newValue);
    }

    g_variant_unref(oldValue);
}

GSettingsItem::GSettingsItem(const QString &key, QObject *parent)
    : QObject (parent)
{
    m_settings = g_settings_new_with_path("org.freedesktop.pulseaudio.module-group", key.toLatin1().data());

    g_signal_connect(m_settings, "changed", G_CALLBACK(GSettingsItem::settingChanged), this);
}

GSettingsItem::~GSettingsItem()
{
    g_settings_sync();
    if (m_settings)
        g_object_unref(m_settings);
}
