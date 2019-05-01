#include "dbwidget.h"
#include "ui_dbwidget.h"

DbWidget::DbWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DbWidget)
{
    ui->setupUi(this);
}

DbWidget::~DbWidget()
{
    delete ui;
}
