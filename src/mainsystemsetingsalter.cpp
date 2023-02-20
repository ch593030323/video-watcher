#include "mainsystemsetingsalter.h"
#include "ui_mainsystemsetingsalter.h"

MainSystemSetingsAlter::MainSystemSetingsAlter(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainSystemSetingsAlter)
{
    ui->setupUi(this);
}

MainSystemSetingsAlter::~MainSystemSetingsAlter()
{
    delete ui;
}
