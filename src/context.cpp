/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "context.h"
#include "server.h"

#include "debug.h"
#include <QAbstractEventDispatcher>
#include <QDBusConnection>
#include <QDBusServiceWatcher>
#include <QGuiApplication>
#include <QIcon>
#include <QTimer>

#include <memory>

#include "card.h"
#include "client.h"
#include "module.h"
#include "sink.h"
#include "sinkinput.h"
#include "source.h"
#include "sourceoutput.h"
#include "streamrestore.h"

#include "context_p.h"
#include "server_p.h"
#include "streamrestore_p.h"

namespace PulseAudioQt
{
qint64 normalVolume()
{
    return PA_VOLUME_NORM;
}

qint64 minimumVolume()
{
    return PA_VOLUME_MUTED;
}

qint64 maximumVolume()
{
    return PA_VOLUME_MAX;
}

qint64 maximumUIVolume()
{
    return PA_VOLUME_UI_MAX;
}

QString ContextPrivate::s_applicationId;

#ifndef K_DOXYGEN

static bool isGoodState(int eol)
{
    if (eol < 0) {
        // Error
        return false;
    }

    if (eol > 0) {
        // End of callback chain
        return false;
    }

    return true;
}

// --------------------------

static void sink_cb(pa_context *context, const pa_sink_info *info, int eol, void *data)
{
    if (!isGoodState(eol))
        return;
    Q_ASSERT(context);
    Q_ASSERT(data);
    static_cast<ContextPrivate *>(data)->sinkCallback(info);
}

static void sink_input_callback(pa_context *context, const pa_sink_input_info *info, int eol, void *data)
{
    if (!isGoodState(eol))
        return;
    // pulsesink probe is used by gst-pulse only to query sink formats (not for playback)
    if (qstrcmp(info->name, "pulsesink probe") == 0) {
        return;
    }
    if (const char *id = pa_proplist_gets(info->proplist, "module-stream-restore.id")) {
        if (qstrcmp(id, "sink-input-by-media-role:event") == 0) {
            qCDebug(PULSEAUDIOQT) << "Ignoring event role sink input.";
            return;
        }
    }
    Q_ASSERT(context);
    Q_ASSERT(data);
    static_cast<ContextPrivate *>(data)->sinkInputCallback(info);
}

static void source_cb(pa_context *context, const pa_source_info *info, int eol, void *data)
{
    if (!isGoodState(eol))
        return;
    // FIXME: This forces excluding monitors
    if (info->monitor_of_sink != PA_INVALID_INDEX)
        return;
    Q_ASSERT(context);
    Q_ASSERT(data);
    static_cast<ContextPrivate *>(data)->sourceCallback(info);
}

static void source_output_cb(pa_context *context, const pa_source_output_info *info, int eol, void *data)
{
    if (!isGoodState(eol))
        return;
    // FIXME: This forces excluding these apps
    if (const char *app = pa_proplist_gets(info->proplist, PA_PROP_APPLICATION_ID)) {
        if (strcmp(app, "org.PulseAudio.pavucontrol") == 0 //
            || strcmp(app, "org.gnome.VolumeControl") == 0 //
            || strcmp(app, "org.kde.kmixd") == 0 //
            || strcmp(app, "org.kde.plasma-pa") == 0) //
            return;
    }
    Q_ASSERT(context);
    Q_ASSERT(data);
    static_cast<ContextPrivate *>(data)->sourceOutputCallback(info);
}

static void client_cb(pa_context *context, const pa_client_info *info, int eol, void *data)
{
    if (!isGoodState(eol))
        return;
    Q_ASSERT(context);
    Q_ASSERT(data);
    static_cast<ContextPrivate *>(data)->clientCallback(info);
}

static void card_cb(pa_context *context, const pa_card_info *info, int eol, void *data)
{
    if (!isGoodState(eol))
        return;
    Q_ASSERT(context);
    Q_ASSERT(data);
    static_cast<ContextPrivate *>(data)->cardCallback(info);
}

static void module_info_list_cb(pa_context *context, const pa_module_info *info, int eol, void *data)
{
    if (!isGoodState(eol))
        return;
    Q_ASSERT(context);
    Q_ASSERT(data);
    static_cast<ContextPrivate *>(data)->moduleCallback(info);
}

static void server_cb(pa_context *context, const pa_server_info *info, void *data)
{
    Q_ASSERT(context);
    Q_ASSERT(data);
    if (!info) {
        // info may be nullptr when e.g. the server doesn't reply in time (e.g. it is stuck)
        // https://bugs.kde.org/show_bug.cgi?id=454647
        qCWarning(PULSEAUDIOQT) << "server_cb() called without info!";
        return;
    }
    static_cast<ContextPrivate *>(data)->serverCallback(info);
}

static void context_state_callback(pa_context *context, void *data)
{
    Q_ASSERT(data);
    static_cast<ContextPrivate *>(data)->contextStateCallback(context);
}

static void subscribe_cb(pa_context *context, pa_subscription_event_type_t type, uint32_t index, void *data)
{
    Q_ASSERT(data);
    static_cast<ContextPrivate *>(data)->subscribeCallback(context, type, index);
}

static void ext_stream_restore_read_cb(pa_context *context, const pa_ext_stream_restore_info *info, int eol, void *data)
{
    if (!isGoodState(eol)) {
        return;
    }
    Q_ASSERT(context);
    Q_ASSERT(data);
    static_cast<ContextPrivate *>(data)->streamRestoreCallback(info);
}

static void ext_stream_restore_subscribe_cb(pa_context *context, void *data)
{
    Q_ASSERT(context);
    Q_ASSERT(data);
    if (!PAOperation(pa_ext_stream_restore_read(context, ext_stream_restore_read_cb, data))) {
        qCWarning(PULSEAUDIOQT) << "pa_ext_stream_restore_read() failed";
    }
}

static void ext_stream_restore_change_sink_cb(pa_context *context, const pa_ext_stream_restore_info *info, int eol, void *data)
{
    if (!isGoodState(eol)) {
        return;
    }
    Q_ASSERT(context);
    Q_ASSERT(data);
    if (qstrncmp(info->name, "sink-input-by", 13) == 0) {
        ContextPrivate *contextp = static_cast<ContextPrivate *>(data);
        const QByteArray deviceData = contextp->m_newDefaultSink.toUtf8();
        pa_ext_stream_restore_info newinfo;
        newinfo.name = info->name;
        newinfo.channel_map = info->channel_map;
        newinfo.volume = info->volume;
        newinfo.mute = info->mute;
        newinfo.device = deviceData.constData();
        contextp->streamRestoreWrite(&newinfo);
    }
}

static void ext_stream_restore_change_source_cb(pa_context *context, const pa_ext_stream_restore_info *info, int eol, void *data)
{
    if (!isGoodState(eol)) {
        return;
    }
    Q_ASSERT(context);
    Q_ASSERT(data);
    if (qstrncmp(info->name, "source-output-by", 16) == 0) {
        ContextPrivate *contextp = static_cast<ContextPrivate *>(data);
        const QByteArray deviceData = contextp->m_newDefaultSource.toUtf8();
        pa_ext_stream_restore_info newinfo;
        newinfo.name = info->name;
        newinfo.channel_map = info->channel_map;
        newinfo.volume = info->volume;
        newinfo.mute = info->mute;
        newinfo.device = deviceData.constData();
        contextp->streamRestoreWrite(&newinfo);
    }
}

#endif

// --------------------------

Context::Context(QObject *parent)
    : QObject(parent)
    , d(new ContextPrivate(this))
{
    d->m_server = new Server(this);
    d->m_context = nullptr;
    d->m_mainloop = nullptr;

    d->connectToDaemon();

    QDBusServiceWatcher *watcher =
        new QDBusServiceWatcher(QStringLiteral("org.pulseaudio.Server"), QDBusConnection::sessionBus(), QDBusServiceWatcher::WatchForRegistration, this);
    connect(watcher, &QDBusServiceWatcher::serviceRegistered, this, [this] {
        d->connectToDaemon();
    });

    connect(&d->m_connectTimer, &QTimer::timeout, this, [this] {
        d->connectToDaemon();
        d->checkConnectTries();
    });

    connect(&d->m_sinks, &MapBaseQObject::added, this, [this](int, QObject *object) {
        Q_EMIT sinkAdded(static_cast<Sink *>(object));
    });
    connect(&d->m_sinks, &MapBaseQObject::removed, this, [this](int, QObject *object) {
        Q_EMIT sinkRemoved(static_cast<Sink *>(object));
    });

    connect(&d->m_sinkInputs, &MapBaseQObject::added, this, [this](int, QObject *object) {
        Q_EMIT sinkInputAdded(static_cast<SinkInput *>(object));
    });
    connect(&d->m_sinkInputs, &MapBaseQObject::removed, this, [this](int, QObject *object) {
        Q_EMIT sinkInputRemoved(static_cast<SinkInput *>(object));
    });

    connect(&d->m_sources, &MapBaseQObject::added, this, [this](int, QObject *object) {
        Q_EMIT sourceAdded(static_cast<Source *>(object));
    });
    connect(&d->m_sources, &MapBaseQObject::removed, this, [this](int, QObject *object) {
        Q_EMIT sourceRemoved(static_cast<Source *>(object));
    });

    connect(&d->m_sourceOutputs, &MapBaseQObject::added, this, [this](int, QObject *object) {
        Q_EMIT sourceOutputAdded(static_cast<SourceOutput *>(object));
    });
    connect(&d->m_sourceOutputs, &MapBaseQObject::removed, this, [this](int, QObject *object) {
        Q_EMIT sourceOutputRemoved(static_cast<SourceOutput *>(object));
    });

    connect(&d->m_clients, &MapBaseQObject::added, this, [this](int, QObject *object) {
        Q_EMIT clientAdded(static_cast<Client *>(object));
    });
    connect(&d->m_clients, &MapBaseQObject::removed, this, [this](int, QObject *object) {
        Q_EMIT clientRemoved(static_cast<Client *>(object));
    });

    connect(&d->m_cards, &MapBaseQObject::added, this, [this](int, QObject *object) {
        Q_EMIT cardAdded(static_cast<Card *>(object));
    });
    connect(&d->m_cards, &MapBaseQObject::removed, this, [this](int, QObject *object) {
        Q_EMIT cardRemoved(static_cast<Card *>(object));
    });

    connect(&d->m_modules, &MapBaseQObject::added, this, [this](int, QObject *object) {
        Q_EMIT moduleAdded(static_cast<Module *>(object));
    });
    connect(&d->m_modules, &MapBaseQObject::removed, this, [this](int, QObject *object) {
        Q_EMIT moduleRemoved(static_cast<Module *>(object));
    });

    connect(&d->m_streamRestores, &MapBaseQObject::added, this, [this](int, QObject *object) {
        Q_EMIT streamRestoreAdded(static_cast<StreamRestore *>(object));
    });
    connect(&d->m_streamRestores, &MapBaseQObject::removed, this, [this](int, QObject *object) {
        Q_EMIT streamRestoreRemoved(static_cast<StreamRestore *>(object));
    });
}

ContextPrivate::ContextPrivate(Context *q)
    : q(q)
{
}

Context::~Context()
{
    delete d;
}

ContextPrivate::~ContextPrivate()
{
    if (m_context) {
        pa_context_unref(m_context);
        m_context = nullptr;
    }

    if (m_mainloop) {
        pa_glib_mainloop_free(m_mainloop);
        m_mainloop = nullptr;
    }

    reset();
}

Context *Context::instance()
{
    static std::unique_ptr<Context> context(new Context);
    return context.get();
}

void ContextPrivate::subscribeCallback(pa_context *context, pa_subscription_event_type_t type, uint32_t index)
{
    Q_ASSERT(context == m_context);

    switch (type & PA_SUBSCRIPTION_EVENT_FACILITY_MASK) {
    case PA_SUBSCRIPTION_EVENT_SINK:
        if ((type & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_REMOVE) {
            m_sinks.removeEntry(index);
        } else {
            if (!PAOperation(pa_context_get_sink_info_by_index(context, index, sink_cb, this))) {
                qCWarning(PULSEAUDIOQT) << "pa_context_get_sink_info_by_index() failed";
                return;
            }
        }
        break;

    case PA_SUBSCRIPTION_EVENT_SOURCE:
        if ((type & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_REMOVE) {
            m_sources.removeEntry(index);
        } else {
            if (!PAOperation(pa_context_get_source_info_by_index(context, index, source_cb, this))) {
                qCWarning(PULSEAUDIOQT) << "pa_context_get_source_info_by_index() failed";
                return;
            }
        }
        break;

    case PA_SUBSCRIPTION_EVENT_SINK_INPUT:
        if ((type & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_REMOVE) {
            m_sinkInputs.removeEntry(index);
        } else {
            if (!PAOperation(pa_context_get_sink_input_info(context, index, sink_input_callback, this))) {
                qCWarning(PULSEAUDIOQT) << "pa_context_get_sink_input_info() failed";
                return;
            }
        }
        break;

    case PA_SUBSCRIPTION_EVENT_SOURCE_OUTPUT:
        if ((type & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_REMOVE) {
            m_sourceOutputs.removeEntry(index);
        } else {
            if (!PAOperation(pa_context_get_source_output_info(context, index, source_output_cb, this))) {
                qCWarning(PULSEAUDIOQT) << "pa_context_get_sink_input_info() failed";
                return;
            }
        }
        break;

    case PA_SUBSCRIPTION_EVENT_CLIENT:
        if ((type & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_REMOVE) {
            m_clients.removeEntry(index);
        } else {
            if (!PAOperation(pa_context_get_client_info(context, index, client_cb, this))) {
                qCWarning(PULSEAUDIOQT) << "pa_context_get_client_info() failed";
                return;
            }
        }
        break;

    case PA_SUBSCRIPTION_EVENT_CARD:
        if ((type & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_REMOVE) {
            m_cards.removeEntry(index);
        } else {
            if (!PAOperation(pa_context_get_card_info_by_index(context, index, card_cb, this))) {
                qCWarning(PULSEAUDIOQT) << "pa_context_get_card_info_by_index() failed";
                return;
            }
        }
        break;

    case PA_SUBSCRIPTION_EVENT_MODULE:
        if ((type & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_REMOVE) {
            m_modules.removeEntry(index);
        } else {
            if (!PAOperation(pa_context_get_module_info_list(context, module_info_list_cb, this))) {
                qCWarning(PULSEAUDIOQT) << "pa_context_get_module_info_list() failed";
                return;
            }
        }
        break;

    case PA_SUBSCRIPTION_EVENT_SERVER:
        if (!PAOperation(pa_context_get_server_info(context, server_cb, this))) {
            qCWarning(PULSEAUDIOQT) << "pa_context_get_server_info() failed";
            return;
        }
        break;
    }
}

void ContextPrivate::contextStateCallback(pa_context *c)
{
    qCDebug(PULSEAUDIOQT) << "state callback";
    pa_context_state_t state = pa_context_get_state(c);
    if (state == PA_CONTEXT_READY) {
        qCDebug(PULSEAUDIOQT) << "ready, stopping connect timer";
        m_connectTimer.stop();

        // 1. Register for the stream changes (except during probe)
        if (m_context == c) {
            pa_context_set_subscribe_callback(c, subscribe_cb, this);

            if (!PAOperation(
                    pa_context_subscribe(c,
                                         (pa_subscription_mask_t)(PA_SUBSCRIPTION_MASK_SINK | PA_SUBSCRIPTION_MASK_SOURCE | PA_SUBSCRIPTION_MASK_CLIENT
                                                                  | PA_SUBSCRIPTION_MASK_SINK_INPUT | PA_SUBSCRIPTION_MASK_SOURCE_OUTPUT
                                                                  | PA_SUBSCRIPTION_MASK_CARD | PA_SUBSCRIPTION_MASK_MODULE | PA_SUBSCRIPTION_MASK_SERVER),
                                         nullptr,
                                         nullptr))) {
                qCWarning(PULSEAUDIOQT) << "pa_context_subscribe() failed";
                return;
            }
        }

        if (!PAOperation(pa_context_get_sink_info_list(c, sink_cb, this))) {
            qCWarning(PULSEAUDIOQT) << "pa_context_get_sink_info_list() failed";
            return;
        }

        if (!PAOperation(pa_context_get_source_info_list(c, source_cb, this))) {
            qCWarning(PULSEAUDIOQT) << "pa_context_get_source_info_list() failed";
            return;
        }

        if (!PAOperation(pa_context_get_client_info_list(c, client_cb, this))) {
            qCWarning(PULSEAUDIOQT) << "pa_context_client_info_list() failed";
            return;
        }

        if (!PAOperation(pa_context_get_card_info_list(c, card_cb, this))) {
            qCWarning(PULSEAUDIOQT) << "pa_context_get_card_info_list() failed";
            return;
        }

        if (!PAOperation(pa_context_get_sink_input_info_list(c, sink_input_callback, this))) {
            qCWarning(PULSEAUDIOQT) << "pa_context_get_sink_input_info_list() failed";
            return;
        }

        if (!PAOperation(pa_context_get_source_output_info_list(c, source_output_cb, this))) {
            qCWarning(PULSEAUDIOQT) << "pa_context_get_source_output_info_list() failed";
            return;
        }

        if (!PAOperation(pa_context_get_module_info_list(c, module_info_list_cb, this))) {
            qCWarning(PULSEAUDIOQT) << "pa_context_get_module_info_list() failed";
            return;
        }

        if (!PAOperation(pa_context_get_server_info(c, server_cb, this))) {
            qCWarning(PULSEAUDIOQT) << "pa_context_get_server_info() failed";
            return;
        }

        if (PAOperation(pa_ext_stream_restore_read(c, ext_stream_restore_read_cb, this))) {
            pa_ext_stream_restore_set_subscribe_cb(c, ext_stream_restore_subscribe_cb, this);
            PAOperation(pa_ext_stream_restore_subscribe(c, 1, nullptr, this));
        } else {
            qCWarning(PULSEAUDIOQT) << "Failed to initialize stream_restore extension";
        }
    } else if (!PA_CONTEXT_IS_GOOD(state)) {
        qCWarning(PULSEAUDIOQT) << "context kaput";
        if (m_context) {
            pa_context_unref(m_context);
            m_context = nullptr;
        }
        reset();
        qCDebug(PULSEAUDIOQT) << "Starting connect timer";
        m_connectTimer.start(std::chrono::seconds(5));
    }
}

void ContextPrivate::sinkCallback(const pa_sink_info *info)
{
    // This parenting here is a bit weird
    m_sinks.updateEntry(info, q);
}

void ContextPrivate::sinkInputCallback(const pa_sink_input_info *info)
{
    m_sinkInputs.updateEntry(info, q);
}

void ContextPrivate::sourceCallback(const pa_source_info *info)
{
    m_sources.updateEntry(info, q);
}

void ContextPrivate::sourceOutputCallback(const pa_source_output_info *info)
{
    m_sourceOutputs.updateEntry(info, q);
}

void ContextPrivate::clientCallback(const pa_client_info *info)
{
    m_clients.updateEntry(info, q);
}

void ContextPrivate::cardCallback(const pa_card_info *info)
{
    m_cards.updateEntry(info, q);
}

void ContextPrivate::moduleCallback(const pa_module_info *info)
{
    m_modules.updateEntry(info, q);
}

void ContextPrivate::streamRestoreCallback(const pa_ext_stream_restore_info *info)
{
    if (qstrcmp(info->name, "sink-input-by-media-role:event") != 0) {
        return;
    }

    const int eventRoleIndex = 1;
    StreamRestore *obj = qobject_cast<StreamRestore *>(m_streamRestores.data().value(eventRoleIndex));

    if (!obj) {
        QVariantMap props;
        props.insert(QStringLiteral("application.icon_name"), QStringLiteral("preferences-desktop-notification"));
        obj = new StreamRestore(eventRoleIndex, props, q);
        obj->d->update(info);
        m_streamRestores.insert(obj);
    } else {
        obj->d->update(info);
    }
}

void ContextPrivate::serverCallback(const pa_server_info *info)
{
    m_server->d->update(info);
}

void Context::setCardProfile(quint32 index, const QString &profile)
{
    if (!d->m_context) {
        return;
    }
    qCDebug(PULSEAUDIOQT) << index << profile;
    if (!PAOperation(pa_context_set_card_profile_by_index(d->m_context, index, profile.toUtf8().constData(), nullptr, nullptr))) {
        qCWarning(PULSEAUDIOQT) << "pa_context_set_card_profile_by_index failed";
        return;
    }
}

void Context::setDefaultSink(const QString &name)
{
    if (!d->m_context) {
        return;
    }
    const QByteArray nameData = name.toUtf8();
    if (!PAOperation(pa_context_set_default_sink(d->m_context, nameData.constData(), nullptr, nullptr))) {
        qCWarning(PULSEAUDIOQT) << "pa_context_set_default_sink failed";
    }

    // Change device for all entries in stream-restore database
    d->m_newDefaultSink = name;
    if (!PAOperation(pa_ext_stream_restore_read(d->m_context, ext_stream_restore_change_sink_cb, d))) {
        qCWarning(PULSEAUDIOQT) << "pa_ext_stream_restore_read failed";
    }
}

void Context::setDefaultSource(const QString &name)
{
    if (!d->m_context) {
        return;
    }
    const QByteArray nameData = name.toUtf8();
    if (!PAOperation(pa_context_set_default_source(d->m_context, nameData.constData(), nullptr, nullptr))) {
        qCWarning(PULSEAUDIOQT) << "pa_context_set_default_source failed";
    }

    // Change device for all entries in stream-restore database
    d->m_newDefaultSource = name;
    if (!PAOperation(pa_ext_stream_restore_read(d->m_context, ext_stream_restore_change_source_cb, d))) {
        qCWarning(PULSEAUDIOQT) << "pa_ext_stream_restore_read failed";
    }
}

void ContextPrivate::streamRestoreWrite(const pa_ext_stream_restore_info *info)
{
    if (!m_context) {
        return;
    }
    if (!PAOperation(pa_ext_stream_restore_write(m_context, PA_UPDATE_REPLACE, info, 1, true, nullptr, nullptr))) {
        qCWarning(PULSEAUDIOQT) << "pa_ext_stream_restore_write failed";
    }
}

void ContextPrivate::connectToDaemon()
{
    if (m_context) {
        return;
    }

    // We require a glib event loop
    if (!QByteArray(QAbstractEventDispatcher::instance()->metaObject()->className()).contains("Glib")) {
        qCWarning(PULSEAUDIOQT) << "Disabling PulseAudio integration for lack of GLib event loop";
        return;
    }

    qCDebug(PULSEAUDIOQT) << "Attempting connection to PulseAudio sound daemon";
    if (!m_mainloop) {
        m_mainloop = pa_glib_mainloop_new(nullptr);
        Q_ASSERT(m_mainloop);
    }

    pa_mainloop_api *api = pa_glib_mainloop_get_api(m_mainloop);
    Q_ASSERT(api);

    pa_proplist *proplist = pa_proplist_new();
    pa_proplist_sets(proplist, PA_PROP_APPLICATION_NAME, QGuiApplication::applicationDisplayName().toUtf8().constData());
    if (!s_applicationId.isEmpty()) {
        pa_proplist_sets(proplist, PA_PROP_APPLICATION_ID, s_applicationId.toUtf8().constData());
    } else {
        pa_proplist_sets(proplist, PA_PROP_APPLICATION_ID, QGuiApplication::desktopFileName().toUtf8().constData());
    }
    pa_proplist_sets(proplist, PA_PROP_APPLICATION_ICON_NAME, QGuiApplication::windowIcon().name().toUtf8().constData());
    m_context = pa_context_new_with_proplist(api, nullptr, proplist);
    pa_proplist_free(proplist);
    Q_ASSERT(m_context);

    if (pa_context_connect(m_context, NULL, PA_CONTEXT_NOFAIL, nullptr) < 0) {
        pa_context_unref(m_context);
        pa_glib_mainloop_free(m_mainloop);
        m_context = nullptr;
        m_mainloop = nullptr;
        return;
    }
    pa_context_set_state_callback(m_context, &context_state_callback, this);
}

void ContextPrivate::checkConnectTries()
{
    if (++m_connectTries == 5) {
        qCWarning(PULSEAUDIOQT) << "Giving up after" << m_connectTries << "tries to connect";
        m_connectTimer.stop();
    }
}

void ContextPrivate::reset()
{
    m_sinks.reset();
    m_sinkInputs.reset();
    m_sources.reset();
    m_sourceOutputs.reset();
    m_clients.reset();
    m_cards.reset();
    m_modules.reset();
    m_streamRestores.reset();
    m_server->reset();
    m_connectTries = 0;
}

bool Context::isValid()
{
    return d->m_context && d->m_mainloop;
}

QVector<Sink *> Context::sinks() const
{
    return d->m_sinks.data();
}

QVector<SinkInput *> Context::sinkInputs() const
{
    return d->m_sinkInputs.data();
}

QVector<Source *> Context::sources() const
{
    return d->m_sources.data();
}

QVector<SourceOutput *> Context::sourceOutputs() const
{
    return d->m_sourceOutputs.data();
}

QVector<Client *> Context::clients() const
{
    return d->m_clients.data();
}

QVector<Card *> Context::cards() const
{
    return d->m_cards.data();
}

QVector<Module *> Context::modules() const
{
    return d->m_modules.data();
}

QVector<StreamRestore *> Context::streamRestores() const
{
    return d->m_streamRestores.data();
}

Server *Context::server() const
{
    return d->m_server;
}

void ContextPrivate::setGenericVolume(
    quint32 index,
    int channel,
    qint64 newVolume,
    pa_cvolume cVolume,
    const std::function<pa_operation *(pa_context *, uint32_t, const pa_cvolume *, pa_context_success_cb_t, void *)> &pa_set_volume)
{
    if (!m_context) {
        return;
    }
    newVolume = qBound<qint64>(0, newVolume, PA_VOLUME_MAX);
    pa_cvolume newCVolume = cVolume;
    if (channel == -1) { // -1 all channels
        const qint64 orig = pa_cvolume_max(&cVolume);
        const qint64 diff = newVolume - orig;
        for (int i = 0; i < newCVolume.channels; ++i) {
            const qint64 channel = newCVolume.values[i];
            const qint64 channelDiff = orig == 0 ? diff : diff * channel / orig;
            newCVolume.values[i] = qBound<qint64>(0, newCVolume.values[i] + channelDiff, PA_VOLUME_MAX);
        }
    } else {
        Q_ASSERT(newCVolume.channels > channel);
        newCVolume.values[channel] = newVolume;
    }
    if (!PAOperation(pa_set_volume(m_context, index, &newCVolume, nullptr, nullptr))) {
        qCWarning(PULSEAUDIOQT) << "pa_set_volume failed";
        return;
    }
}

void ContextPrivate::setGenericMute(quint32 index,
                                    bool mute,
                                    const std::function<pa_operation *(pa_context *, uint32_t, int, pa_context_success_cb_t, void *)> &pa_set_mute)
{
    if (!m_context) {
        return;
    }
    if (!PAOperation(pa_set_mute(m_context, index, mute, nullptr, nullptr))) {
        qCWarning(PULSEAUDIOQT) << "pa_set_mute failed";
        return;
    }
}

void ContextPrivate::setGenericPort(quint32 index,
                                    const QString &portName,
                                    const std::function<pa_operation *(pa_context *, uint32_t, const char *, pa_context_success_cb_t, void *)> &pa_set_port)
{
    if (!m_context) {
        return;
    }
    if (!PAOperation(pa_set_port(m_context, index, portName.toUtf8().constData(), nullptr, nullptr))) {
        qCWarning(PULSEAUDIOQT) << "pa_set_port failed";
        return;
    }
}

void ContextPrivate::setGenericDeviceForStream(
    quint32 streamIndex,
    quint32 deviceIndex,
    const std::function<pa_operation *(pa_context *, uint32_t, uint32_t, pa_context_success_cb_t, void *)> &pa_move_stream_to_device)
{
    if (!m_context) {
        return;
    }
    if (!PAOperation(pa_move_stream_to_device(m_context, streamIndex, deviceIndex, nullptr, nullptr))) {
        qCWarning(PULSEAUDIOQT) << "pa_move_stream_to_device failed";
        return;
    }
}

void ContextPrivate::setGenericVolumes(
    quint32 index,
    QVector<qint64> channelVolumes,
    pa_cvolume cVolume,
    const std::function<pa_operation *(pa_context *, uint32_t, const pa_cvolume *, pa_context_success_cb_t, void *)> &pa_set_volume)
{
    if (!m_context) {
        return;
    }
    Q_ASSERT(channelVolumes.count() == cVolume.channels);

    pa_cvolume newCVolume = cVolume;
    for (int i = 0; i < channelVolumes.count(); ++i) {
        newCVolume.values[i] = qBound<qint64>(0, channelVolumes.at(i), PA_VOLUME_MAX);
    }

    if (!PAOperation(pa_set_volume(m_context, index, &newCVolume, nullptr, nullptr))) {
        qCWarning(PULSEAUDIOQT) << "pa_set_volume failed";
        return;
    }
}

void Context::setApplicationId(const QString &applicationId)
{
    ContextPrivate::s_applicationId = applicationId;
}

pa_context *Context::context() const
{
    return d->m_context;
}

} // PulseAudioQt
