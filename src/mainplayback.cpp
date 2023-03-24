#include "mainplayback.h"
#include "ui_mainplayback.h"
#include "lds.h"

MainPlayBack::MainPlayBack(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainPlayBack)
{
    ui->setupUi(this);this->setWindowTitle(lds::windowName);
}

MainPlayBack::~MainPlayBack()
{
    delete ui;
}
