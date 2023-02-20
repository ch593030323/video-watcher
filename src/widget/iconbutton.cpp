#include "iconbutton.h"
#include "propertycolor.h"

#include <QPainter>
#include <QStyle>

IconButton::IconButton(QWidget *parent) : QWidget(parent)
{
    color = PropertyColor::buttonColor;
}

void IconButton::setIconChar(QChar ch)
{
    this->ch = ch;
}

void IconButton::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QPixmap pixmap = PropertyColor::getFontPixmap(ch, color, this->size());
    painter.drawPixmap(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, pixmap.size(), this->rect()), pixmap);
}

void IconButton::mousePressEvent(QMouseEvent *event)
{
    color = PropertyColor::highlightColor;
    update();
}

void IconButton::mouseReleaseEvent(QMouseEvent *event)
{
    color = PropertyColor::buttonHoverColor;
    update();
}

void IconButton::enterEvent(QEvent *event)
{
    color = PropertyColor::buttonHoverColor;
    update();
}

void IconButton::leaveEvent(QEvent *event)
{
    color = PropertyColor::buttonColor;
    update();
}
