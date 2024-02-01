/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "pulseobject.h"
#include "pulseobject_p.h"

#include "context.h"

#include <QIcon>

namespace PulseAudioQt
{
PulseObject::PulseObject(QObject *parent)
    : QObject(parent)
    , d(new PulseObjectPrivate(this))
{
}

PulseObject::~PulseObject()
{
    delete d;
}

PulseObjectPrivate::PulseObjectPrivate(PulseObject *q)
    : q(q)
{
}

PulseObjectPrivate::~PulseObjectPrivate()
{
}

QString PulseObject::name() const
{
    return d->m_name;
}

QString PulseObject::iconName() const
{
    QString name = d->m_properties.value(QStringLiteral("device.icon_name")).toString();
    if (!name.isEmpty() && QIcon::hasThemeIcon(name)) {
        return name;
    }

    name = d->m_properties.value(QStringLiteral("media.icon_name")).toString();
    if (!name.isEmpty() && QIcon::hasThemeIcon(name)) {
        return name;
    }

    name = d->m_properties.value(QStringLiteral("window.icon_name")).toString();
    if (!name.isEmpty() && QIcon::hasThemeIcon(name)) {
        return name;
    }

    name = d->m_properties.value(QStringLiteral("application.icon_name")).toString();
    if (!name.isEmpty() && QIcon::hasThemeIcon(name)) {
        return name;
    }

    name = d->m_properties.value(QStringLiteral("application.process.binary")).toString();
    if (!name.isEmpty() && QIcon::hasThemeIcon(name)) {
        return name;
    }

    name = d->m_properties.value(QStringLiteral("application.name")).toString();
    if (!name.isEmpty() && QIcon::hasThemeIcon(name)) {
        return name;
    }

    name = d->m_properties.value(QStringLiteral("pipewire.access.portal.app_id")).toString();
    if (!name.isEmpty() && QIcon::hasThemeIcon(name)) {
        return name;
    }

    name = property("name").toString();
    if (!name.isEmpty() && QIcon::hasThemeIcon(name)) {
        return name;
    }

    return QString();
}

QVariantMap PulseObject::properties() const
{
    return d->m_properties;
}

} // PulseAudioQt
