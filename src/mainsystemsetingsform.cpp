#include "mainsystemsetingsform.h"
#include "ui_mainsystemsetingsform.h"

#include "propertycolor.h"
#include "playformnewdialog.h"

MainSystemSetingsForm::MainSystemSetingsForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainSystemSetingsForm)
{
    ui->setupUi(this);
    ui->pushButton_add->setIcon(PropertyColor::getFontPixmap(0x2b));
    ui->pushButton_del->setIcon(PropertyColor::getFontPixmap(0x2d));
    ui->pushButton_modfy->setIcon(PropertyColor::getFontPixmap(0x2a));

    connect(ui->pushButton_add, SIGNAL(clicked()),this,SLOT(toadd()));
    connect(ui->pushButton_modfy, SIGNAL(clicked()),this,SLOT(tomodfy()));
    connect(ui->pushButton_del, SIGNAL(clicked()),this,SLOT(todel()));
}

MainSystemSetingsForm::~MainSystemSetingsForm()
{
    delete ui;
}

void MainSystemSetingsForm::toadd()
{
   playformnewdialog d(this);
   d.resize(832, 557);
   d.exec();
}

void MainSystemSetingsForm::todel()
{

}

void MainSystemSetingsForm::tomodfy()
{

}
