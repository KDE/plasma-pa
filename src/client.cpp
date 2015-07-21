#include "client.h"

#include <QDebug>

namespace QPulseAudio
{

Client::Client(QObject *parent)
    : PulseObject(parent)
{
}

Client::~Client()
{
}

void Client::update(const pa_client_info *info)
{
    updatePulseObject(info);

    QString infoName = QString::fromUtf8(info->name);
    if (m_name != infoName) {
        m_name = infoName;
        emit nameChanged();
    }
}

} // QPulseAudio
