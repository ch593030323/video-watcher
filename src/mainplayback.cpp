#include "mainplayback.h"
#include "ui_mainplayback.h"

MainPlayBack::MainPlayBack(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainPlayBack)
{
    ui->setupUi(this);
}

MainPlayBack::~MainPlayBack()
{
    delete ui;
}
