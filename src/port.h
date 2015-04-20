#ifndef PORT_H
#define PORT_H

#include "profile.h"

class Q_DECL_EXPORT Port : public Profile
{
    Q_OBJECT
    Q_PROPERTY(bool available READ isAvailable NOTIFY availableChanged)
public:
    Port(QObject *parent);
    virtual ~Port();

    template<typename PAInfo>
    void setInfo(const PAInfo *info)
    {
        Profile::setInfo(info);
        if (m_isAvailable != info->available) {
            m_isAvailable = info->available;
            emit availableChanged();
        }
    }

    bool isAvailable() const;

signals:
    void availableChanged();

private:
    bool m_isAvailable;
};

#endif // PORT_H
