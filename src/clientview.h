#ifndef CLIENTVIEW_H
#define CLIENTVIEW_H

#include <QWidget>

namespace Ui {
class ClientView;
}

class Context;
class QAbstractItemModel;

class ClientView : public QWidget
{
    Q_OBJECT

public:
    explicit ClientView(Context *context, QWidget *parent = 0);
    ~ClientView();
    void setModel(QAbstractItemModel *model);

private:
    Ui::ClientView *ui;
};

#endif // CLIENTVIEW_H
