#include "autocloselabel.h"

#include <QtDebug>

AutoCloseLabel::AutoCloseLabel(QWidget *parent)
    : QLabel(parent)
{

}

AutoCloseLabel::~AutoCloseLabel()
{
}

void AutoCloseLabel::mousePressEvent(QMouseEvent *e)
{
    QLabel::mousePressEvent(e);
    this->close();
}

void AutoCloseLabel::hideEvent(QHideEvent *e)
{
    QLabel::hideEvent(e);
    this->close();
}
