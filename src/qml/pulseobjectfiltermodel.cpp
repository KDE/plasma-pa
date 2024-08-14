/*
    SPDX-FileCopyrightText: 2024 ivan tkachenko <me@ratijas.tk>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "pulseobjectfiltermodel.h"

#include <PulseAudioQt/Device>
#include <PulseAudioQt/Models>
#include <PulseAudioQt/Port>

using namespace PulseAudioQt;
using namespace Qt::Literals::StringLiterals;

// DEFAULT_SINK_NAME in module-always-sink.c
constexpr auto &&DUMMY_OUTPUT_NAME = "auto_null"_L1;

PulseObjectFilter::PulseObjectFilter() = default;

PulseObjectFilter::PulseObjectFilter(const QString &role, const QVariant &value)
    : role(role)
    , value(value)
{
}

bool PulseObjectFilter::operator==(const PulseObjectFilter &other) const
{
    return role == other.role && value == other.value;
}

QDebug operator<<(QDebug debug, const PulseObjectFilter &filter)
{
    return debug.noquote() << "PulseObjectFilter(role=" << filter.role << ", value=" << filter.value << ")";
}

PulseObjectFilter PulseObjectFilterModel::makeFilter(const QString &role, const QVariant &value)
{
    return PulseObjectFilter(role, value);
}

const QList<PulseObjectFilter> &PulseObjectFilterModel::filters() const
{
    return m_filters;
}

void PulseObjectFilterModel::setFilters(const QList<PulseObjectFilter> &filters)
{
    if (m_filters != filters) {
        m_filters = filters;
        invalidateFilter();
        Q_EMIT filtersChanged();
    }
}

bool PulseObjectFilterModel::filterOutInactiveDevices() const
{
    return m_filterOutInactiveDevices;
}

void PulseObjectFilterModel::setFilterOutInactiveDevices(bool filter)
{
    if (m_filterOutInactiveDevices != filter) {
        m_filterOutInactiveDevices = filter;
        invalidateFilter();
        Q_EMIT filterOutInactiveDevicesChanged();
    }
}

bool PulseObjectFilterModel::filterVirtualDevices() const
{
    return m_filterVirtualDevices;
}

void PulseObjectFilterModel::setFilterVirtualDevices(bool filter)
{
    if (m_filterVirtualDevices != filter) {
        m_filterVirtualDevices = filter;
        invalidateFilter();
        Q_EMIT filterVirtualDevicesChanged();
    }
}

bool PulseObjectFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    const auto sourceModel = this->sourceModel();
    if (!sourceModel) {
        return false;
    }

    const auto idx = sourceModel->index(sourceRow, 0, sourceParent);

    // Don't ever show the dummy output, that's silly
    const auto NameRole = sourceModel->roleNames().key("Name"_ba);
    if (sourceModel->data(idx, NameRole) == DUMMY_OUTPUT_NAME) {
        return false;
    }

    // Optionally run the role-based filters
    for (const auto &filter : std::as_const(m_filters)) {
        const auto FilterRole = sourceModel->roleNames().key(filter.role.toLatin1());
        if (sourceModel->data(idx, FilterRole) != filter.value) {
            return false;
        }
    }

    // Optionally exclude inactive devices
    if (m_filterOutInactiveDevices) {
        if (auto device = sourceModel->data(idx, AbstractModel::PulseObjectRole).value<Device *>()) {
            const auto ports = device->ports();
            if (ports.length() == 1 && ports[0]->availability() == Profile::Unavailable) {
                return false;
            }
        }
    }

    if (m_filterVirtualDevices) {
        if (auto device = sourceModel->data(idx, AbstractModel::PulseObjectRole).value<Device *>()) {
            return !device->isVirtualDevice();
        }
    }

    return true;
}

#include "moc_pulseobjectfiltermodel.cpp"
