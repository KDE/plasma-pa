#include "clientview.h"
#include "ui_clientview.h"

#include "context.h"
#include "pulseaudio.h"

ClientView::ClientView(Context *context, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClientView)
{
    ui->setupUi(this);
}

ClientView::~ClientView()
{
    delete ui;
}

void ClientView::setModel(QAbstractItemModel *model)
{
    ui->listView->setModel(model);
}
