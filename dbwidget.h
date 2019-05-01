#ifndef DBWIDGET_H
#define DBWIDGET_H

#include <QWidget>

namespace Ui {
class DbWidget;
}

class DbWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DbWidget(QWidget *parent = 0);
    ~DbWidget();

private:
    Ui::DbWidget *ui;
};

#endif // DBWIDGET_H
