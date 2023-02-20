#include "videoinspanel.h"
#include "ui_videoinspanel.h"
#include "propertycolor.h"

VideoInsPanel::VideoInsPanel(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::VideoInsPanel)
{
    ui->setupUi(this);
    ui->widget_2->setIconChar(0xf00e);
    ui->widget_3->setIconChar(0xf010);

}

VideoInsPanel::~VideoInsPanel()
{
    delete ui;
}

void VideoInsPanel::setStepValue(int value)
{
    ui->horizontalSlider->setValue(value);
    ui->horizontalSlider->setTipValue(value);
}

void VideoInsPanel::resizeEvent(QResizeEvent *event)
{
    ui->horizontalSlider->updateTipPos();
}
