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

static void client_cb(pa_context *context, const pa_client_info *info, int eol, void *data)
{
    if (!isGoodState(eol))
        return;
    Q_ASSERT(context);
    Q_ASSERT(data);
    ((Context *)data)->clientCallback(info);
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
    connectToDaemon();
}

Context::~Context()
{
    reset();
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
                    const int modelIndex = m_sinks.keys().indexOf(index);
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
                    const int modelIndex = m_sinkInputs.keys().indexOf(index);
                    m_sinkInputs.take(index)->deleteLater();
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
                    m_recentlyDeletedClients.insert(index);
                } else {
                    const int modelIndex = m_clients.keys().indexOf(index);
                    m_clients.take(index)->deleteLater();
                    emit clientRemoved(modelIndex);
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
        reset();
#warning do reconnect here I guess
    }
}

template <typename C, typename PAInfo, typename AddedSignal, typename UpdatedSignal>
void Context::updateMap(QMap<quint32, C *> &map,
                        QSet<quint32> &exclusionSet,
                        const PAInfo *info,
                        AddedSignal addedSignal,
                        UpdatedSignal updatedSignal)
{
    Q_ASSERT(info);

    if (exclusionSet.remove(info->index)) {
        // Was already removed again.
        return;
    }

    const bool isNew = !map.contains(info->index);

    auto *obj = map.value(info->index, nullptr);
    if (!obj) {
        obj = new C;
    }
    obj->setInfo(info);
    map.insert(info->index, obj);

    const int modelIndex = map.keys().indexOf(info->index);
    Q_ASSERT(modelIndex >= 0);

    if (isNew) {
        emit (this->*addedSignal)(modelIndex);
    } else {
        emit (this->*updatedSignal)(modelIndex);
    }
}

#warning fixme recentlydeleted has the same type for everything making it easy to use the wrong one possibly
void Context::sinkCallback(const pa_sink_info *info)
{
    updateMap<Sink, pa_sink_info>(m_sinks, m_recentlyDeletedSinks,
                                  info,
                                  &Context::sinkAdded,
                                  &Context::sinkUpdated);
}

void Context::clientCallback(const pa_client_info *info)
{
    updateMap<Client, pa_client_info>(m_clients, m_recentlyDeletedClients,
                                      info,
                                      &Context::clientAdded,
                                      &Context::clientUpdated);
}

void Context::sinkInputCallback(const pa_sink_input_info *info)
{
    updateMap<SinkInput, pa_sink_input_info>(m_sinkInputs, m_recentlyDeletedSinkInputs,
                                             info,
                                             &Context::sinkInputAdded,
                                             &Context::sinkInputUpdated);
}

void Context::setSinkVolume(quint32 index, quint32 volume)
{
    Sink *obj = m_sinks.value(index, nullptr);
    if (!obj)
        return;
    setGenericVolume(index, volume, obj->volume(), &pa_context_set_sink_volume_by_index);
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
    SinkInput *obj = m_sinkInputs.value(index, nullptr);
    if (!obj)
        return;
    setGenericVolume(index, volume, obj->volume(), &pa_context_set_sink_input_volume);
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
template <typename PAFunction>
void Context::setGenericVolume(quint32 index, quint32 newVolume,
                               pa_cvolume cVolume, PAFunction pa_set_volume)
{
    qDebug() << Q_FUNC_INFO << index << newVolume;
    #warning fixme volume limit enforcement needs review for sensibility also this prevents overdrive
    if (newVolume > 65536)
        newVolume = 0;
    pa_cvolume newCVolume = cVolume;
    for (int i = 0; i < newCVolume.channels; ++i) {
        newCVolume.values[i] = newVolume;
    }
    pa_operation *o;
    if (!(o = pa_set_volume(m_context, index, &newCVolume, NULL, NULL))) {
        qWarning() <<  "pa_context_set_sink_volume_by_index() failed";
        return;
    }
    pa_operation_unref(o);
}

#warning make this a member
template <typename Map, typename Set>
static void _wipeAll(Map &map, Set &set)
{
    qDeleteAll(map);
    map.clear();
    set.clear();
}

void Context::reset()
{
    pa_context_unref(m_context);
    m_context = nullptr;

    pa_glib_mainloop_free(m_mainloop);
    m_mainloop = nullptr;

#warning introduce a super structure to loop over so we cannot forget things by accident
    _wipeAll(m_sinks, m_recentlyDeletedSinks);
    _wipeAll(m_sinkInputs, m_recentlyDeletedSinkInputs);
    _wipeAll(m_clients, m_recentlyDeletedClients);
}
