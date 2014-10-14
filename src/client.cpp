#include "client.h"

#include <QDebug>

Client::Client()
{
}

void Client::setInfo(const pa_client_info *info)
{
    m_name = QString::fromUtf8(info->name);
    m_ownerModule = info->owner_module;
    m_driver = QString::fromUtf8(info->driver);

    m_properties.clear();

    void *it = nullptr;
    while (const char *key = pa_proplist_iterate(info->proplist, &it)) {
        Q_ASSERT(key);
        const char *value = pa_proplist_gets(info->proplist, key);
        if (!value) {
            qDebug() << "property" << key << "not a string";
            continue;
        }
        Q_ASSERT(value);
        m_properties.insert(QString::fromUtf8(key), QString::fromUtf8(value));
        qDebug() << key << value;
    }
}
