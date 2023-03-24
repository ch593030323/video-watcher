#include "videoinspanel.h"
#include "ui_videoinspanel.h"
#include "propertycolor.h"
#include "lds.h"

VideoInsPanel::VideoInsPanel(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::VideoInsPanel)
{
    ui->setupUi(this);this->setWindowTitle(lds::windowName);
    ui->horizontalSlider->setFocusPolicy(Qt::NoFocus);


    ui->button_focus_length_add->setIconChar(0xf055);
    ui->button_focus_length_sub->setIconChar(0xf056);
    ui->button_aperture_add->setIconChar(0xf055);
    ui->button_aperture_sub->setIconChar(0xf056);
    ui->button_focus_add->setIconChar(0xf055);
    ui->button_focus_sub->setIconChar(0xf056);

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
