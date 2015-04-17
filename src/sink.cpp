#include "sink.h"

#include "context.h"

void Sink::update(const pa_sink_info *info)
{
    updateDevice(info);
}

void Sink::setVolume(qint64 volume)
{
    context()->setSinkVolume(index(), volume);
}

void Sink::setMuted(bool muted)
{
    context()->setGenericMute(m_index, muted, &pa_context_set_sink_mute_by_index);
}

void Sink::setActivePortIndex(quint32 port_index)
{
    qDebug() << port_index;
    Port *port = qobject_cast<Port *>(ports().at(port_index));
    if (!port) {
        qWarning() << "invalid port set request" << port_index;
        return;
    }
    context()->setGenericPort(index(), port->name(), &pa_context_set_sink_port_by_index);
}
