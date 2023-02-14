#include "propertycolor.h"
#include <QVariant>
#include <QString>
#include <QApplication>
#include <QPainter>
#include <QStyle>

QColor PropertyColor::highlightColor;
QColor PropertyColor::textColor;
QColor PropertyColor::borderColor;
QColor PropertyColor::subToolColor;
QColor PropertyColor::buttonColor;
QColor PropertyColor::buttonTextColor;

QString PropertyColor::iconFontFamily;

QPixmap PropertyColor::getFontPixmap(QChar ch, const QColor &color, const QSize &size)
{
    QFont font = qApp->font();
    font.setFamily(iconFontFamily);
    font.setPixelSize(size.height());

    QPixmap pix(size);
    pix.fill(Qt::transparent);

    QPainter painter(&pix);
    painter.setFont(font);
    painter.setPen(color);
    painter.drawText(pix.rect(), Qt::AlignCenter, ch);

    return pix;
}

void PropertyColor::sethighlightColor(const QColor &color)
{
    highlightColor = color;
}

QColor PropertyColor::gethighlightColor() const
{
    return highlightColor;
}

void PropertyColor::settextColor(const QColor &color)
{
    textColor = color;
}

QColor PropertyColor::gettextColor() const
{
    return textColor;
}

void PropertyColor::setborderColor(const QColor &color)
{
    borderColor = color;
}

QColor PropertyColor::getborderColor() const
{
    return borderColor;
}

void PropertyColor::setsubToolColor(const QColor &color)
{
    subToolColor = color;
}

QColor PropertyColor::getsubToolColor() const
{
    return subToolColor;
}

void PropertyColor::setbuttonColor(const QColor &color)
{
    buttonColor = color;
}

QColor PropertyColor::getbuttonColor() const
{
    return buttonColor;
}

void PropertyColor::setbuttonTextColor(const QColor &color)
{
    buttonTextColor = color;
}

QColor PropertyColor::getbuttonTextColor() const
{
    return buttonTextColor;
}

void PropertyColor::init(const QString &iconFontFamily, const QString &styleSheetString)
{
    PropertyColor::iconFontFamily = iconFontFamily;
    PropertyColor qsswidget;
    qsswidget.setStyleSheet(styleSheetString);
    qsswidget.style()->polish(&qsswidget);
}

PropertyColor::PropertyColor(QWidget *parent) : QWidget(parent)
{
    setProperty("outer_stylesheet", QString("PropertyColor"));
}
