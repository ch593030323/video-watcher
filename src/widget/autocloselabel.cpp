#include "autocloselabel.h"

#include <QtDebug>

AutoCloseLabel::AutoCloseLabel(QWidget *parent)
    : QLabel(parent)
{

}

AutoCloseLabel::~AutoCloseLabel()
{
    qDebug() << __FUNCTION__;
}

void AutoCloseLabel::mousePressEvent(QMouseEvent *)
{
    this->close();
}
