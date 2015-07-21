#include "volumeosd.h"

#include "osdservice.h"

#define SERVICE QLatin1Literal("org.kde.plasmashell")
#define PATH QLatin1Literal("/org/kde/osdService")
#define CONNECTION QDBusConnection::sessionBus()

VolumeOSD::VolumeOSD(QObject *parent)
    : QObject(parent)
{
}

void VolumeOSD::show(int percent)
{
    OsdServiceInterface osdService(SERVICE, PATH, CONNECTION);
    osdService.volumeChanged(percent);
}
