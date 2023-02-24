#include "iconbutton.h"
#include "propertycolor.h"

#include <QPainter>
#include <QStyle>
#include <QMouseEvent>

IconButton::IconButton(QWidget *parent) : QWidget(parent)
{
    color = PropertyColor::buttonColor;
    type = NormalButton;
}

void IconButton::setIconChar(QChar ch)
{
    this->ch = ch;
}

void IconButton::setType(IconButton::Type type)
{
    this->type = type;
    color = PropertyColor::buttonTextColor;
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
    if(type == ToolButton)
        color = PropertyColor::buttonTextColor;

    update();
    if(this->rect().contains(event->pos())) {
        emit clicked();
    }
}

void IconButton::enterEvent(QEvent *event)
{
    color = PropertyColor::buttonHoverColor;
    if(type == ToolButton)
        color = PropertyColor::buttonTextColor;
    update();
}

void IconButton::leaveEvent(QEvent *event)
{
    color = PropertyColor::buttonColor;
    if(type == ToolButton)
        color = PropertyColor::buttonTextColor;
    update();
}
