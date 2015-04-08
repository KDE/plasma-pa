#ifndef SOURCE_H
#define SOURCE_H

#include "devicebase.h"

class Q_DECL_EXPORT Source : public DeviceBase<pa_source_info, pa_source_port_info>
{
    Q_OBJECT
};

#endif // SOURCE_H
