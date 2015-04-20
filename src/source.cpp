#include "source.h"

#include "context.h"

Source::Source(QObject *parent)
    : Device(parent)
{
}

void Source::update(const pa_source_info *info)
{
    updateDevice(info);
}

void Source::setVolume(qint64 volume)
{
    context()->setGenericVolume(index(), volume, cvolume(), &pa_context_set_source_volume_by_index);
}

void Source::setMuted(bool muted)
{
    context()->setGenericMute(index(), muted, &pa_context_set_source_mute_by_index);
}

void Source::setActivePortIndex(quint32 port_index)
{
    Port *port = qobject_cast<Port *>(ports().at(port_index));
    if (!port) {
        qWarning() << "invalid port set request" << port_index;
        return;
    }
    context()->setGenericPort(index(), port->name(), &pa_context_set_source_port_by_index);
}
