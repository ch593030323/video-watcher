#include "lineeditfind.h"
#include "propertycolor.h"

#include <QHBoxLayout>
#include <QApplication>
#include <QPainter>

LineEditFind::LineEditFind(QWidget *parent) : QFrame(parent)
{
    edit = new QLineEdit;
    buttonFind = new QPushButton;
    buttonClose = new QPushButton;

    buttonFind->setFlat(true);
    buttonClose->setFlat(true);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(buttonFind);
    layout->addWidget(edit, 1);
    layout->addWidget(buttonClose);
    setLayout(layout);

    this->setProperty("outer_stylesheet", "search");
    edit->setProperty("outer_stylesheet", "search");
    buttonFind->setProperty("outer_stylesheet", "search");
    buttonClose->setProperty("outer_stylesheet", "search");

    buttonFind->setIcon(PropertyColor::getFontPixmap(0xf002, PropertyColor::textColor, QSize(20, 20)));
    buttonClose->setIcon(PropertyColor::getFontPixmap(0xf057, PropertyColor::textColor, QSize(20, 20)));


    connect(edit, SIGNAL(textChanged(QString)), this, SLOT(slotShowCloseButton(QString)));
    connect(buttonClose, SIGNAL(clicked()), edit, SLOT(clear()));

    buttonClose->hide();
}

QLineEdit *LineEditFind::lineEdit()
{
    return edit;
}

void LineEditFind::hideFindButton()
{
    buttonFind->hide();
}

void LineEditFind::setText(const QString &text)
{
    edit->setText(text);
}

QString LineEditFind::text()
{
    return edit->text();
}

void LineEditFind::slotShowCloseButton(const QString &text)
{
     buttonClose->setVisible(text.count() > 0);
}
