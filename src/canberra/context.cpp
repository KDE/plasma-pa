#include "context.h"

#include <QDebug>

namespace QCanberraPrivate {

static void finish_cb(ca_context *c, uint32_t id, int error_code, void *userdata)
{
    Q_UNUSED(c);
    Q_ASSERT(userdata);
    PendingEvent *event = static_cast<PendingEvent *>(userdata);
    Q_ASSERT(event);
    Q_ASSERT(event->id() == id);
    // This callback may be called from any thread. Must force it to go into the
    // event loop via invokeMethod to prevent critical code from getting exposed
    // if and when the callback thread is not the event thread.
    QMetaObject::invokeMethod(event, "onFinish", Qt::QueuedConnection, Q_ARG(int, error_code));
}

Context::Context(QObject *parent) :
    QObject(parent)
{
    int err = ca_context_create(&m_context);
    if (err < 0) {
        m_context = nullptr;
        return;
    }
    ca_context_set_driver(m_context, "pulse");
}

Context::~Context()
{
    if (m_context) {
        ca_context_destroy(m_context);
    }
}

bool Context::isValid() const
{
    return m_context != nullptr;
}

void Context::play(quint32 index)
{
    qDebug() << index;
    int playing = 0;
    int cindex = 2; // Note "2" is simply the index we've picked. It's somewhat irrelevant.
    ca_context_playing(m_context, cindex, &playing);

    // NB Depending on how this is desired to work, we may want to simply
    // skip playing, or cancel the currently playing sound and play our
    // new one... for now, let's do the latter.
    if (playing) {
        ca_context_cancel(m_context, cindex);
        playing = 0;
    }

    char dev[64];

    snprintf(dev, sizeof(dev), "%lu", (unsigned long) index);
    ca_context_change_device(m_context, dev);

    // Ideally we'd use something like ca_gtk_play_for_widget()...
    ca_context_play(
        m_context,
        cindex,
        CA_PROP_EVENT_DESCRIPTION, "Volume Control Feedback Sound",
        CA_PROP_EVENT_ID, "audio-volume-change",
        CA_PROP_CANBERRA_CACHE_CONTROL, "permanent",
        CA_PROP_CANBERRA_ENABLE, "1",
        nullptr
    );

    ca_context_change_device(m_context, nullptr);
}

PendingEvent::PendingEvent(QObject *parent)
    : QObject(parent)
{
}

PendingEvent::~PendingEvent()
{
}

quint32 PendingEvent::id() const
{
    return m_id;
}

bool PendingEvent::finished() const
{
    return m_finished;
}

void PendingEvent::onFinished(int errorCode)
{
    // FIXME: map errocode
    m_finished = true;
    emit errorChanged();
    emit finishedChanged();
}



} // namespace QCanberraPrivate
