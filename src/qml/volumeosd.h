#ifndef VOLUMEOSD_H
#define VOLUMEOSD_H

#include <QObject>

class VolumeOSD : public QObject
{
    Q_OBJECT
public:
    VolumeOSD(QObject *parent = nullptr);

public slots:
    void show(int percent);
};

#endif // VOLUMEOSD_H
