#include "context.h"

#include <QAbstractEventDispatcher>
#include <QDebug>
#include <QMutexLocker>

#include "card.h"
#include "client.h"
#include "operation.h"
#include "sink.h"
#include "sinkinput.h"
#include "source.h"
#include "sourceoutput.h"

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
    ((Context *)data)->sinkCallback(info);
}

static void sink_input_callback(pa_context *context, const pa_sink_input_info *info, int eol, void *data)
{
    if (!isGoodState(eol))
        return;
    Q_ASSERT(context);
    Q_ASSERT(data);
    ((Context *)data)->sinkInputCallback(info);
}

static void source_cb(pa_context *context, const pa_source_info *info, int eol, void *data)
{
    if (!isGoodState(eol))
        return;
#warning force excluding monitors
    if (info->monitor_of_sink != PA_INVALID_INDEX)
        return;
    Q_ASSERT(context);
    Q_ASSERT(data);
    ((Context *)data)->sourceCallback(info);
}

static void source_output_cb(pa_context *context, const pa_source_output_info *info, int eol, void *data)
{
    if (!isGoodState(eol))
        return;
#warning force excluding random apps
    if (const char *app = pa_proplist_gets(info->proplist, PA_PROP_APPLICATION_ID)) {
        if (strcmp(app, "org.PulseAudio.pavucontrol") == 0
                || strcmp(app, "org.gnome.VolumeControl") == 0
                || strcmp(app, "org.kde.kmixd") == 0)
            return;
    }
    Q_ASSERT(context);
    Q_ASSERT(data);
    ((Context *)data)->sourceOutputCallback(info);
}

static void client_cb(pa_context *context, const pa_client_info *info, int eol, void *data)
{
    if (!isGoodState(eol))
        return;
    Q_ASSERT(context);
    Q_ASSERT(data);
    ((Context *)data)->clientCallback(info);
}

static void card_cb(pa_context *context, const pa_card_info *info, int eol, void *data)
{
    if (!isGoodState(eol))
        return;
    Q_ASSERT(context);
    Q_ASSERT(data);
    ((Context *)data)->cardCallback(info);
}

static void context_state_callback(pa_context *context, void *data)
{
    Q_ASSERT(data);
    ((Context *)data)->contextStateCallback(context);
}

static void subscribe_cb(pa_context *context, pa_subscription_event_type_t type, uint32_t index, void *data)
{
    Q_ASSERT(data);
    ((Context *)data)->subscribeCallback(context, type, index);
}

// --------------------------

Context::Context(QObject *parent)
    : QObject(parent)
    , m_context(nullptr)
    , m_mainloop(nullptr)
    , m_references(0)
{
    connectToDaemon();
}

Context::~Context()
{
    qDebug() << "cherrio old chap!";
    reset();
}

Context *Context::instance()
{
    static Context *context = new Context;
    return context;
}

void Context::ref()
{
    ++m_references;
    qDebug() << m_references << "☎☎☎☎☎☎☎☎☎☎☎☎☎☎☎☎☎☎☎☎☎☎☎☎☎☎☎☎☎☎☎☎☎☎☎☎☎☎☎☎☎☎";
}

void Context::unref()
{
    qDebug() << m_references-1 << "んんんんんんんんんんんんんんんんんんんんんんんんんんんんんんんんんんんんんんんんんんんんんんんんんんんんんんん";
    if (--m_references == 0) {
        delete this;
    }
}

void Context::subscribeCallback(pa_context *context, pa_subscription_event_type_t type, uint32_t index)
{
    Q_ASSERT(context == m_context);

    switch (type & PA_SUBSCRIPTION_EVENT_FACILITY_MASK) {
    case PA_SUBSCRIPTION_EVENT_SINK:
        if ((type & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_REMOVE) {
            m_sinks.removeEntry(index);
        } else {
            if (!PAOperation(pa_context_get_sink_info_by_index(context, index, sink_cb, this))) {
                qWarning() << "pa_context_get_sink_info_by_index() failed";
                return;
            }
        }
        break;

    case PA_SUBSCRIPTION_EVENT_SOURCE:
        if ((type & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_REMOVE) {
            m_sources.removeEntry(index);
        } else {
            if (!PAOperation(pa_context_get_source_info_by_index(context, index, source_cb, this))) {
                qWarning() << "pa_context_get_source_info_by_index() failed";
                return;
            }
        }
        break;

    case PA_SUBSCRIPTION_EVENT_SINK_INPUT:
        if ((type & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_REMOVE) {
            m_sinkInputs.removeEntry(index);
        } else {
            if (!PAOperation(pa_context_get_sink_input_info(context, index, sink_input_callback, this))) {
                qWarning() << "pa_context_get_sink_input_info() failed";
                return;
            }
        }
        break;

    case PA_SUBSCRIPTION_EVENT_SOURCE_OUTPUT:
        if ((type & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_REMOVE) {
            m_sourceOutputs.removeEntry(index);
        } else {
            if (!PAOperation(pa_context_get_source_output_info(context, index, source_output_cb, this))) {
                qWarning() << "pa_context_get_sink_input_info() failed";
                return;
            }
        }
        break;

    case PA_SUBSCRIPTION_EVENT_CLIENT:
        if ((type & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_REMOVE) {
            m_clients.removeEntry(index);
        } else {
            if (!PAOperation(pa_context_get_client_info(context, index, client_cb, this))) {
                qWarning() << "pa_context_get_client_info() failed";
                return;
            }
        }
        break;

#warning maybe point out to upstream that their api is inconsistent _by_index vs. null
    case PA_SUBSCRIPTION_EVENT_CARD:
        if ((type & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_REMOVE) {
            m_cards.removeEntry(index);
        } else {
            if (!PAOperation(pa_context_get_card_info_by_index(context, index, card_cb, this))) {
                qWarning() << "pa_context_get_card_info_by_index() failed";
                return;
            }
        }
        break;

    }
}

void Context::contextStateCallback(pa_context *c)
{
    qDebug() << "state callback";
    pa_context_state_t state = pa_context_get_state(c);
    if (state == PA_CONTEXT_READY) {
        qDebug() << "ready";

        // 1. Register for the stream changes (except during probe)
        if (m_context == c) {
            pa_context_set_subscribe_callback(c, subscribe_cb, this);

            if (!PAOperation(pa_context_subscribe(c, (pa_subscription_mask_t)
                                           (PA_SUBSCRIPTION_MASK_SINK|
                                            PA_SUBSCRIPTION_MASK_SOURCE|
                                            PA_SUBSCRIPTION_MASK_CLIENT|
                                            PA_SUBSCRIPTION_MASK_SINK_INPUT|
                                            PA_SUBSCRIPTION_MASK_SOURCE_OUTPUT), NULL, NULL))) {
                qWarning() << "pa_context_subscribe() failed";
                return;
            }
        }

        if (!PAOperation(pa_context_get_sink_info_list(c, sink_cb, this))) {
            qWarning() << "pa_context_get_sink_info_list() failed";
            return;
        }

        if (!PAOperation(pa_context_get_source_info_list(c, source_cb, this))) {
            qWarning() << "pa_context_get_source_info_list() failed";
            return;
        }

        if (!PAOperation(pa_context_get_client_info_list(c, client_cb, this))) {
            qWarning() << "pa_context_client_info_list() failed";
            return;
        }

        if (!PAOperation(pa_context_get_card_info_list(c, card_cb, this))) {
            qWarning() << "pa_context_get_card_info_list() failed";
            return;
        }

        if (!PAOperation(pa_context_get_sink_input_info_list(c, sink_input_callback, this))) {
            qWarning() << "pa_context_get_sink_input_info_list() failed";
            return;
        }

        if (!PAOperation(pa_context_get_source_output_info_list(c, source_output_cb, this))) {
            qWarning() << "pa_context_get_source_output_info_list() failed";
            return;
        }

#warning todo
        /* These calls are not always supported */
        //        if ((o = pa_ext_stream_restore_read(c, ext_stream_restore_read_cb, NULL))) {
        //            pa_operation_unref(o);

        //            pa_ext_stream_restore_set_subscribe_cb(c, ext_stream_restore_subscribe_cb, NULL);

        //            if ((o = pa_ext_stream_restore_subscribe(c, 1, NULL, NULL)))
        //                pa_operation_unref(o);
        //        } else {
        //            qWarning() << "Failed to initialize stream_restore extension: " << pa_strerror(pa_context_errno(m_context));
        //        }
    } else if (!PA_CONTEXT_IS_GOOD(state)) {
        qDebug() << "context kaput";
        reset();
#warning do reconnect here I guess
    }
}

#warning fixme recentlydeleted has the same type for everything making it easy to use the wrong one possibly
void Context::sinkCallback(const pa_sink_info *info)
{
    m_sinks.updateEntry(info);
}

void Context::sinkInputCallback(const pa_sink_input_info *info)
{
    m_sinkInputs.updateEntry(info);
}

void Context::sourceCallback(const pa_source_info *info)
{
    m_sources.updateEntry(info);
}

void Context::sourceOutputCallback(const pa_source_output_info *info)
{
    m_sourceOutputs.updateEntry(info);
}

void Context::clientCallback(const pa_client_info *info)
{
    m_clients.updateEntry(info);
}

void Context::cardCallback(const pa_card_info *info)
{
    m_cards.updateEntry(info);
}

void Context::setSinkVolume(quint32 index, qint64 volume)
{
    Sink *obj = m_sinks.data().value(index, nullptr);
    if (!obj)
        return;
    setGenericVolume(index, volume, obj->cvolume(), &pa_context_set_sink_volume_by_index);
}

#warning sesetgenericmute is not outside linkable unless we have a use here...
void Context::setSinkMute(quint32 index, bool mute)
{
    Sink *obj = m_sinks.data().value(index, nullptr);
    if (!obj)
        return;
    setGenericMute(index, mute, &pa_context_set_sink_mute_by_index);
}

void Context::setSinkPort(quint32 index, const QString &portName)
{
    if (!PAOperation(pa_context_set_sink_port_by_index(m_context,
                                                       index,
                                                       portName.toUtf8().constData(),
                                                       nullptr,
                                                       nullptr))) {
        qWarning() << "pa_context_set_sink_port_by_index failed";
        return;
    }
}

void Context::setSinkInputVolume(quint32 index, qint64 volume)
{
    SinkInput *obj = m_sinkInputs.data().value(index, nullptr);
    if (!obj)
        return;
    setGenericVolume(index, volume, obj->volume(), &pa_context_set_sink_input_volume);
}

void Context::setSinkInputSinkByModelIndex(quint32 index, int sinkModelIndex)
{
    qint64 sinkIndex = m_sinks.dataIndexToPaIndex(sinkModelIndex);
    if (sinkIndex == -1) {
        qWarning() << "Tried to switch SinkInput" << sinkIndex << "to non-existant SinkModelIndex" << sinkModelIndex;
        return;
    }

    qDebug() << Q_FUNC_INFO << index <<  sinkIndex;
    SinkInput *obj = m_sinkInputs.data().value(index, nullptr);
    if (!obj)
        return;
    if (!PAOperation(pa_context_move_sink_input_by_index(m_context,
                                                         index,
                                                         sinkIndex,
                                                         nullptr,
                                                         nullptr))) {
        qWarning() << "pa_context_move_sink_input_by_index failed";
        return;
    }
}

void Context::setSourceVolume(quint32 index, qint64 volume)
{
    Source *obj = m_sources.data().value(index, nullptr);
    if (!obj)
        return;
    setGenericVolume(index, volume, obj->cvolume(), &pa_context_set_source_volume_by_index);
}

void Context::setSourceOutputVolume(quint32 index, qint64 volume)
{
    SourceOutput *obj = m_sourceOutputs.data().value(index, nullptr);
    if (!obj)
        return;
    setGenericVolume(index, volume, obj->volume(), &pa_context_set_source_output_volume);
}

void Context::setSourceOutputSinkByModelIndex(quint32 index, int sourceModelIndex)
{
    qint64 sourceIndex = m_sources.dataIndexToPaIndex(sourceModelIndex);
    if (sourceIndex == -1) {
        qWarning() << "Tried to switch Source" << index << "to non-existant sourceModelIndex" << sourceModelIndex;
        return;
    }

    qDebug() << Q_FUNC_INFO << index <<  sourceIndex;
    SourceOutput *obj = m_sourceOutputs.data().value(index, nullptr);
    if (!obj)
        return;
    if (!PAOperation(pa_context_move_source_output_by_index(m_context,
                                                            index,
                                                            sourceIndex,
                                                            nullptr,
                                                            nullptr))) {
        qWarning() << "pa_context_move_source_output_by_index failed";
        return;
    }
}

void Context::setCardProfile(quint32 index, const QString &profileName)
{
    qDebug() << Q_FUNC_INFO << index << profileName;
    Card *obj = m_cards.data().value(index, nullptr);
    if (!obj)
        return;
    if (!PAOperation(pa_context_set_card_profile_by_index(m_context,
                                                          index,
                                                          profileName.toUtf8().constData(),
                                                          nullptr, nullptr))) {
        qWarning() << "pa_context_set_card_profile_by_index failed";
        return;
    }
}

void Context::connectToDaemon()
{
    Q_ASSERT(m_context == nullptr);
    Q_ASSERT(m_mainloop == nullptr);

    // We require a glib event loop
    if (!QByteArray(QAbstractEventDispatcher::instance()->metaObject()->className()).contains("EventDispatcherGlib")) {
        qDebug() << "Disabling PulseAudio integration for lack of GLib event loop";
        return;
    }

    qDebug() <<  "Attempting connection to PulseAudio sound daemon";
    m_mainloop = pa_glib_mainloop_new(nullptr);
    Q_ASSERT(m_mainloop);
    pa_mainloop_api *api = pa_glib_mainloop_get_api(m_mainloop);
    Q_ASSERT(api);

    m_context = pa_context_new(api, "QPulse");
    Q_ASSERT(m_context);

    if (pa_context_connect(m_context, NULL, PA_CONTEXT_NOFAIL, nullptr) < 0) {
        pa_context_unref(m_context);
        pa_glib_mainloop_free(m_mainloop);
        m_context = nullptr;
        return;
    }
    pa_context_set_state_callback(m_context, &context_state_callback, this);
}

#warning volume and mute should not assume success but wait for callback

template <typename PAFunction>
void Context::setGenericVolume(quint32 index, qint64 newVolume,
                               pa_cvolume cVolume, PAFunction pa_set_volume)
{
    qDebug() << Q_FUNC_INFO << index << newVolume;
#warning fixme volume limit enforcement needs review for sensibility also this prevents overdrive
    newVolume = qBound<qint64>(0, newVolume, 65536);
    pa_cvolume newCVolume = cVolume;
    for (int i = 0; i < newCVolume.channels; ++i) {
        newCVolume.values[i] = newVolume;
    }
    if (!PAOperation(pa_set_volume(m_context, index, &newCVolume, NULL, NULL))) {
        qWarning() <<  "pa_context_set_sink_volume_by_index() failed";
        return;
    }
}

template <typename PAFunction>
void Context::setGenericMute(quint32 index, bool mute, PAFunction pa_set_mute)
{
    qDebug() << Q_FUNC_INFO << index << mute;
    if (!PAOperation(pa_set_mute(m_context, index, mute, nullptr, nullptr))) {
        qWarning() <<  "pa_set_mute() failed";
        return;
    }
}

void Context::reset()
{
    pa_context_unref(m_context);
    m_context = nullptr;

    pa_glib_mainloop_free(m_mainloop);
    m_mainloop = nullptr;

    m_sinks.reset();
    m_sinkInputs.reset();
    m_sources.reset();
    m_sourceOutputs.reset();
    m_clients.reset();
}
