/*
    SPDX-FileCopyrightText: 2018 Nicolas Fella <nicolas.fella@gmx.de>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/
#ifndef STREAMRESTORE_P_H
#define STREAMRESTORE_P_H

#include "streamrestore.h"
#include <QVector>
#include <pulse/ext-stream-restore.h>

namespace PulseAudioQt
{
class StreamRestorePrivate
{
public:
    explicit StreamRestorePrivate(StreamRestore *q);
    virtual ~StreamRestorePrivate();

    void writeChanges(const pa_cvolume &volume, bool muted, const QString &device);
    void update(const pa_ext_stream_restore_info *info);

    StreamRestore *q;
    QString m_device;
    pa_cvolume m_volume;
    pa_channel_map m_channelMap;
    QVector<QString> m_channels;
    bool m_muted = false;
    quint32 m_index = 0;

    struct {
        bool valid = false;
        pa_cvolume volume;
        bool muted;
        QString device;
    } m_cache;
};

}

#endif
