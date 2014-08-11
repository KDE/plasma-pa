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

#warning clear properties first?
    void *it = nullptr;
    while (const char *key = pa_proplist_iterate(info->proplist, &it)) {
        const char *value = pa_proplist_gets(info->proplist, key);
        if (!value) {
            qDebug() << "property" << key << "not a string";
            continue;
        }
        setProperty(key, QString::fromUtf8(value));
//        qDebug() << key << value
    }
}
