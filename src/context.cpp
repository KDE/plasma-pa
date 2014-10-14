#include "context.h"

#include <QAbstractEventDispatcher>
#include <QDebug>

#warning todo this needs to be a singleton as multiple contexts dont make sense and it makes it eaier to use from qml

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
    Q_ASSERT(data);
    ((Context *)data)->sinkCallback(context, info, eol);
}

static void sink_input_callback(pa_context *context, const pa_sink_input_info *info, int eol, void *data)
{
    if (!isGoodState(eol))
        return;
    Q_ASSERT(data);
    ((Context *)data)->sinkInputCallback(context, info, eol);
}

static void client_cb(pa_context *context, const pa_client_info *info, int eol, void *data)
{
    if (!isGoodState(eol))
        return;
    Q_ASSERT(data);
    ((Context *)data)->clientCallback(context, info, eol);
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
{
    // We require a glib event loop
    if (!QByteArray(QAbstractEventDispatcher::instance()->metaObject()->className()).contains("EventDispatcherGlib")) {
        qDebug() << "Disabling PulseAudio integration for lack of GLib event loop";
        return;
    }

    connectToDaemon();
}

Context::~Context()
{
    pa_context_unref(m_context);
    pa_glib_mainloop_free(m_mainloop);
}

void Context::subscribeCallback(pa_context *context, pa_subscription_event_type_t type, uint32_t index)
{
#warning fixme
//    Q_ASSERT(c == s_context);

    switch (type & PA_SUBSCRIPTION_EVENT_FACILITY_MASK) {
#warning fixme
        case PA_SUBSCRIPTION_EVENT_SINK:
            if ((type & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_REMOVE) {
                qDebug() << "DEL Sink" << index;
                if (!m_sinks.contains(index)) {
                    m_recentlyDeletedSinks.insert(index);
                } else {
                    int modelIndex = m_sinks.keys().indexOf(index);
                    m_sinks.take(index)->deleteLater();
                    emit sinkRemoved(modelIndex);
                }
            } else {
                pa_operation *o;
                if (!(o = pa_context_get_sink_info_by_index(context, index, sink_cb, this))) {
                    qWarning() << "pa_context_get_sink_info_by_index() failed";
                    return;
                }
                pa_operation_unref(o);
            }
            break;

//        case PA_SUBSCRIPTION_EVENT_SOURCE:
//            if ((t & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_REMOVE) {
//                if (s_mixers.contains(KMIXPA_CAPTURE))
//                    s_mixers[KMIXPA_CAPTURE]->removeWidget(index);
//            } else {
//                pa_operation *o;
//                if (!(o = pa_context_get_source_info_by_index(c, index, source_cb, NULL))) {
//                    qWarning() << "pa_context_get_source_info_by_index() failed";
//                    return;
//                }
//                pa_operation_unref(o);
//            }
//            break;

        case PA_SUBSCRIPTION_EVENT_SINK_INPUT:
            if ((type & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_REMOVE) {
                qDebug() << Q_FUNC_INFO << "::: dropping :::" << index;
                if (!m_sinkInputs.contains(index)) {
                    m_recentlyDeletedSinkInputs.insert(index);
                } else {
                    qDebug() << "dropping sink input" << index;
                    qDebug() << m_sinkInputs.count();
                    int modelIndex = m_sinkInputs.keys().indexOf(index);
                    m_sinkInputs.take(index)->deleteLater();
                    qDebug() << m_sinkInputs.count();
                    emit sinkInputRemoved(modelIndex);
                }
            } else {
                pa_operation *o;
                if (!(o = pa_context_get_sink_input_info(context, index, sink_input_callback, this))) {
                    qWarning() << "pa_context_get_sink_input_info() failed";
                    return;
                }
                pa_operation_unref(o);
            }
            break;

//        case PA_SUBSCRIPTION_EVENT_SOURCE_OUTPUT:
//            if ((t & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_REMOVE) {
//                if (s_mixers.contains(KMIXPA_APP_CAPTURE))
//                    s_mixers[KMIXPA_APP_CAPTURE]->removeWidget(index);
//            } else {
//                pa_operation *o;
//                if (!(o = pa_context_get_source_output_info(c, index, source_output_cb, NULL))) {
//                    qWarning() << "pa_context_get_sink_input_info() failed";
//                    return;
//                }
//                pa_operation_unref(o);
//            }
//            break;

        case PA_SUBSCRIPTION_EVENT_CLIENT:
            if ((type & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_REMOVE) {
                qDebug() << "dropping client" << index;
                if (!m_clients.contains(index)) {
                    m_recentDeletedClients.insert(index);
                } else {
                    m_clients.take(index)->deleteLater();
                    emit clientsChanged();
                }
            } else {
                pa_operation *o;
                if (!(o = pa_context_get_client_info(context, index, client_cb, this))) {
                    qWarning() << "pa_context_get_client_info() failed";
                    return;
                }
                pa_operation_unref(o);
            }
            break;

    }
    qDebug() << "return !!!!!";
}

void Context::contextStateCallback(pa_context *c)
{
    qDebug() << "state callback";
    pa_context_state_t state = pa_context_get_state(c);
    if (state == PA_CONTEXT_READY) {
        qDebug() << "ready";
        // Attempt to load things up
        pa_operation *o = nullptr;

        // 1. Register for the stream changes (except during probe)
        if (m_context == c) {
#warning fixme
            pa_context_set_subscribe_callback(c, subscribe_cb, this);

            if (!(o = pa_context_subscribe(c, (pa_subscription_mask_t)
                                           (PA_SUBSCRIPTION_MASK_SINK|
                                            PA_SUBSCRIPTION_MASK_SOURCE|
                                            PA_SUBSCRIPTION_MASK_CLIENT|
                                            PA_SUBSCRIPTION_MASK_SINK_INPUT|
                                            PA_SUBSCRIPTION_MASK_SOURCE_OUTPUT), NULL, NULL))) {
                qWarning() << "pa_context_subscribe() failed";
                return;
            }
            pa_operation_unref(o);
        }

#warning fixme
        if (!(o = pa_context_get_sink_info_list(c, sink_cb, this))) {
            qWarning() << "pa_context_get_sink_info_list() failed";
            return;
        }
        pa_operation_unref(o);

//        if (!(o = pa_context_get_source_info_list(c, source_cb, NULL))) {
//            qWarning() << "pa_context_get_source_info_list() failed";
//            return;
//        }
//        pa_operation_unref(o);

        if (!(o = pa_context_get_client_info_list(c, client_cb, this))) {
            qWarning() << "pa_context_client_info_list() failed";
            return;
        }
        pa_operation_unref(o);

        if (!(o = pa_context_get_sink_input_info_list(c, sink_input_callback, this))) {
            qWarning() << "pa_context_get_sink_input_info_list() failed";
            return;
        }
        pa_operation_unref(o);

//        if (!(o = pa_context_get_source_output_info_list(c, source_output_cb, NULL))) {
//            qWarning() << "pa_context_get_source_output_info_list() failed";
//            return;
//        }
//        pa_operation_unref(o);

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
        // If this is our probe phase, exit our context immediately
        if (m_context != c) {
            pa_context_disconnect(c);
        } else {
            // If we're not probing, it means we've been disconnected from our
            // glib context
            pa_context_unref(m_context);
            m_context = nullptr;

#warning fixme
            // This one is not handled above.
//            clients.clear();
        }
    }
}

#warning data changed should possibly only be emitted once eol is >0 to avoid pointless redraws

void Context::sinkCallback(pa_context *context, const pa_sink_info *info, int eol)
{
    Q_ASSERT(info);

    qDebug() << "sink_cb" << info->index << info->name;
    bool isNew = false;

    if (m_recentlyDeletedSinks.remove(info->index)) {
        // Was already removed again.
        return;
    }

    Sink *obj = m_sinks.value(info->index, nullptr);
    if (!obj) {
        obj = new Sink;
        isNew = true;
    }
    obj->setInfo(info);
    m_sinks.insert(info->index, obj);

#warning this is bullshit and removal by this method is even more shit
    int listIndex = -1;
    QList<quint32> list = m_sinks.keys();
    for (int i = 0; i < list.size(); ++i) {
        if (list.at(i) == info->index) {
            listIndex = i;
            break;
        }
    }
    Q_ASSERT(listIndex >= 0);

    if (isNew) {
        emit sinkAdded(listIndex);
    } else {
        emit sinkUpdated(listIndex);
    }
}

void Context::clientCallback(pa_context *context, const pa_client_info *info, int eol)
{
    Q_ASSERT(context);
    Q_ASSERT(info);

    if (m_recentDeletedClients.remove(info->index)) {
        // Was already removed again.
        return;
    }

    Client *obj = m_clients.value(info->index, nullptr);
    if (!obj)
        obj = new Client;
    obj->setInfo(info);
    m_clients.insert(info->index, obj);

    emit clientsChanged();
}

void Context::sinkInputCallback(pa_context *context, const pa_sink_input_info *info, int eol)
{
    Q_ASSERT(context);
    Q_ASSERT(info);

    bool isNew = false;

    if (m_recentlyDeletedSinkInputs.remove(info->index)) {
        // Was already removed again.
        return;
    }

    SinkInput *obj = m_sinkInputs.value(info->index, nullptr);
    if (!obj) {
        obj = new SinkInput;
        isNew = true;
    }
    obj->setInfo(info);
    m_sinkInputs.insert(info->index, obj);

    int listIndex = -1;
    QList<quint32> list = m_sinkInputs.keys();
    for (int i = 0; i < list.size(); ++i) {
        if (list.at(i) == info->index) {
            listIndex = i;
            break;
        }
    }
    Q_ASSERT(listIndex >= 0);

    qDebug() << Q_FUNC_INFO << isNew << listIndex;
    if (isNew) {
        emit sinkInputAdded(listIndex);
    } else {
        emit sinkInputUpdated(listIndex);
    }
}

void Context::setSinkVolume(quint32 index, quint32 volume)
{
    qDebug() << Q_FUNC_INFO << index << volume;
    pa_operation *o;
    Sink *sink = m_sinks.value(index, nullptr);
    if (!sink)
        return;
    pa_cvolume newVolume = sink->volume();
    for (int i = 0; i < newVolume.channels; ++i) {
        newVolume.values[i] = volume;
    }
    if (!(o = pa_context_set_sink_volume_by_index(m_context, index, &newVolume, NULL, NULL))) {
        qWarning() <<  "pa_context_set_sink_volume_by_index() failed";
        return;
    }
    pa_operation_unref(o);
}

void Context::setSinkPort(quint32 portIndex)
{
    pa_operation *o;
    if (!(o = pa_context_set_sink_port_by_index(m_context, portIndex, nullptr, nullptr, nullptr))) {
        qWarning() << "pa_context_set_sink_port_by_index failed";
        return;
    }
}

void Context::setSinkInputVolume(quint32 index, quint32 volume)
{
    qDebug() << Q_FUNC_INFO << index << volume;
    pa_operation *o;
    SinkInput *obj = m_sinkInputs.value(index, nullptr);
    if (!obj)
        return;
    pa_cvolume newVolume = obj->volume();
    for (int i = 0; i < newVolume.channels; ++i) {
        newVolume.values[i] = volume;
    }
    if (!(o = pa_context_set_sink_input_volume(m_context, index, &newVolume, NULL, NULL))) {
        qWarning() <<  "pa_context_set_sink_volume_by_index() failed";
        return;
    }
    pa_operation_unref(o);
}

void Context::connectToDaemon()
{
    Q_ASSERT(m_context == nullptr);
    Q_ASSERT(m_mainloop == nullptr);

    qDebug() <<  "Attempting connection to PulseAudio sound daemon";
    m_mainloop = pa_glib_mainloop_new(nullptr);
    pa_mainloop_api *api = pa_glib_mainloop_get_api(m_mainloop);
    Q_ASSERT(api);

    m_context = pa_context_new(api, "QPulse");
    Q_ASSERT(m_context);

    if (pa_context_connect(m_context, NULL, PA_CONTEXT_NOFAIL, 0) < 0) {
        pa_context_unref(m_context);
        m_context = nullptr;
        return;
    }
    pa_context_set_state_callback(m_context, &context_state_callback, this);
}
