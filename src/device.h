#ifndef DEVICE_H
#define DEVICE_H

#include <QString>

#include <pulse/volume.h>

#include "volumeobject.h"
#include "port.h"
#include "pulseobject.h"

class Q_DECL_EXPORT Device : public VolumeObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString description READ description NOTIFY descriptionChanged)
    Q_PROPERTY(quint32 cardIndex READ cardIndex NOTIFY cardIndexChanged)
    Q_PROPERTY(QList<QObject *> ports READ ports NOTIFY portsChanged)
    Q_PROPERTY(quint32 activePortIndex READ activePortIndex WRITE setActivePortIndex NOTIFY activePortIndexChanged)
public:
    virtual ~Device() {}

    template <typename PAInfo>
    void updateDevice(const PAInfo *info)
    {
        updateVolumeObject(info);

        if (m_name != info->name) {
            m_name = info->name;
            emit nameChanged();
        }
        if (m_description != info->description) {
            m_description = info->description;
            emit descriptionChanged();
        }

        m_cardIndex = info->card;
        emit cardIndexChanged();

        // TODO: this rebuilds the entire port list on every update. would be
        // nicer if it actually removed what needs removing updates what needs
        // updating and adds what needs adding. Alas, this is a tad more
        // involved.
        qDeleteAll(m_ports);
        m_ports.clear();
        for (auto **ports = info->ports; ports && *ports != nullptr; ++ports) {
            Port *port = new Port(this);
            port->setInfo(*ports);
            m_ports.append(port);
            if (info->active_port == *ports) {
                m_activePortIndex = m_ports.length() - 1;
            }
        }
        emit portsChanged();
        emit activePortIndexChanged();
    }

    QString name() const { return m_name; }
    QString description() const { return m_description; }
    quint32 cardIndex() const { return m_cardIndex; }
    QList<QObject *> ports() const { return m_ports; }
    quint32 activePortIndex() const { return m_activePortIndex; }
    virtual void setActivePortIndex(quint32 port_index) = 0;

signals:
    void nameChanged();
    void descriptionChanged();
    void cardIndexChanged();
    void portsChanged();
    void activePortIndexChanged();

protected:
    Device(QObject *parent) : VolumeObject(parent) {}

private:
    QString m_name;
    QString m_description;
    quint32 m_cardIndex = -1;
    QList<QObject *> m_ports;
    quint32 m_activePortIndex = -1;
};

#endif // DEVICE_H
