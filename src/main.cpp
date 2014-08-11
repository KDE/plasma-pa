#include <QApplication>

#include "clientview.h"
#include "context.h"
#include "pulseaudio.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Context pulse;

    ClientView v(&pulse);
    v.setModel(new ClientModel(&pulse));
    v.show();

    ClientView v1(&pulse);
    v1.setModel(new SinkInputModel(&pulse));
    v1.show();

    return app.exec();
}

