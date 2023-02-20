#include "tipslider.h"
#include "propertycolor.h"

#include <QStyleOptionSlider>
#include <QToolTip>
#include <QTimer>

TipSlider::TipSlider(QWidget * parent)
    : QSlider(parent)
{
    tip = new QLabel(this);
    tip->setProperty("outer_stylesheet", "sliderTip");
    tip->show();
    tip->setAlignment(Qt::AlignCenter);
}

TipSlider::TipSlider(Qt::Orientation orientation, QWidget * parent)
    : QSlider(orientation, parent)
{
}

void TipSlider::setTipValue(int value)
{
    tip->setNum(value);
    QTimer::singleShot(0, this, SLOT(updateTipPos()));
}

void TipSlider::updateTipPos()
{
    sliderChange(QAbstractSlider::SliderValueChange);
}

void TipSlider::sliderChange(QAbstractSlider::SliderChange change)
{
    QSlider::sliderChange(change);

    if (change == QAbstractSlider::SliderValueChange )
    {
        QStyleOptionSlider opt;
        initStyleOption(&opt);

        QRect sr = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);
        tip->setGeometry(sr);
        tip->setText(QString::number(value()));
    }
}
