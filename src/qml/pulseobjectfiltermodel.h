/*
    SPDX-FileCopyrightText: 2024 ivan tkachenko <me@ratijas.tk>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#pragma once

#include <QDebug>
#include <QList>
#include <QObject>
#include <QSortFilterProxyModel>

struct PulseObjectFilter {
    Q_GADGET

    Q_PROPERTY(QString role MEMBER role)
    Q_PROPERTY(QVariant value MEMBER value)

public:
    explicit PulseObjectFilter();
    explicit PulseObjectFilter(const QString &role, const QVariant &value);

    bool operator==(const PulseObjectFilter &other) const;

private:
    friend class PulseObjectFilterModel;
    friend QDebug operator<<(QDebug debug, const PulseObjectFilter &filter);

    QString role;
    QVariant value;
};

class PulseObjectFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT

    Q_PROPERTY(QList<PulseObjectFilter> filters READ filters WRITE setFilters NOTIFY filtersChanged)
    Q_PROPERTY(bool filterOutInactiveDevices READ filterOutInactiveDevices WRITE setFilterOutInactiveDevices NOTIFY filterOutInactiveDevicesChanged)
    Q_PROPERTY(bool filterVirtualDevices READ filterVirtualDevices WRITE setFilterVirtualDevices NOTIFY filterVirtualDevicesChanged)

public:
    using QSortFilterProxyModel::QSortFilterProxyModel;

    Q_INVOKABLE [[nodiscard]] static PulseObjectFilter makeFilter(const QString &role, const QVariant &value);

    [[nodiscard]] const QList<PulseObjectFilter> &filters() const;
    void setFilters(const QList<PulseObjectFilter> &filters);

    [[nodiscard]] bool filterOutInactiveDevices() const;
    void setFilterOutInactiveDevices(bool filter);

    [[nodiscard]] bool filterVirtualDevices() const;
    void setFilterVirtualDevices(bool filter);

Q_SIGNALS:
    void filtersChanged();
    void filterOutInactiveDevicesChanged();
    void filterVirtualDevicesChanged();

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private:
    QList<PulseObjectFilter> m_filters;
    bool m_filterOutInactiveDevices = false;
    bool m_filterVirtualDevices = false;
};
