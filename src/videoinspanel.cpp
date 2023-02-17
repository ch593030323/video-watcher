#include "videoinspanel.h"
#include "ui_videoinspanel.h"
#include "propertycolor.h"

VideoInsPanel::VideoInsPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VideoInsPanel)
{
    ui->setupUi(this);

    ui->pushButton_apert_add->setIcon(PropertyColor::getFontPixmap(0xf055));
    ui->pushButton_focus_add->setIcon(PropertyColor::getFontPixmap(0xf055));
    ui->pushButton_value_add->setIcon(PropertyColor::getFontPixmap(0xf055));
    ui->pushButton_apert_add->setFlat(true);
    ui->pushButton_focus_add->setFlat(true);
    ui->pushButton_value_add->setFlat(true);

    ui->pushButton_apert_sub->setIcon(PropertyColor::getFontPixmap(0xf056));
    ui->pushButton_focus_sub->setIcon(PropertyColor::getFontPixmap(0xf056));
    ui->pushButton_value_sub->setIcon(PropertyColor::getFontPixmap(0xf056));
    ui->pushButton_apert_sub->setFlat(true);
    ui->pushButton_focus_sub->setFlat(true);
    ui->pushButton_value_sub->setFlat(true);
}

VideoInsPanel::~VideoInsPanel()
{
    delete ui;
}
