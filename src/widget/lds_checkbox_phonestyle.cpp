#include "lds_checkbox_phonestyle.h"
#include <QVariant>
#include <QStylePainter>
#include <QStyleOptionButton>
#include "lds.h"
#include <QtDebug>

lds_checkbox_phonestyle::lds_checkbox_phonestyle(QWidget *parent) :
    QPushButton(parent)
{
    QString check_png_align = "background-position: right;";
    if(this->layoutDirection() == Qt::RightToLeft) {
        check_png_align = "background-position: left;";
    }
    icon_width = lds::getFontPixmap(0xf204).width();
    this->setStyleSheet(
                "        QPushButton{text-align : left;min-height:30px;background:transparent;}"
                "        QPushButton:checked{"
                "            background-image: url(:/skin_image/light/check_on_60x40.png);"
                "            background-repeat:no-repeat;" + check_png_align +
                "            border:0px solid black;"
                "        }"
                "        QPushButton:!checked{"
                "            background-image: url(:/skin_image/light/check_off_60x40.png);"
                "            background-repeat:no-repeat;" + check_png_align +
                "            border:0px solid black;"
                "        }        "
                );
    this->setCheckable(true);
    this->setProperty("outer_stylesheet", QVariant("pushbutton_check"));
}

void lds_checkbox_phonestyle::setUnderline(bool underline)
{
    QFont f = this->font();
    f.setUnderline(underline);
    this->setFont(f);
}

void lds_checkbox_phonestyle::setData(int role, const QVariant &value)
{
    map.insert(role, value);
}

QVariant lds_checkbox_phonestyle::getData(int role)
{
    return map.value(role);
}

void lds_checkbox_phonestyle::paintEvent(QPaintEvent *)
{
    QStylePainter p(this);
    QStyleOptionButton option;
    initStyleOption(&option);
    option.text = "";
    p.drawControl(QStyle::CE_PushButton, option);
    option.rect.adjust(0, 0, -icon_width, 0);
    option.text = this->text();
    p.drawText(option.rect, Qt::AlignLeft | Qt::AlignVCenter | Qt::TextWordWrap, option.text);
}
