#include "lineeditx.h"

#include <QIntValidator>

LineEditX::LineEditX(QWidget *parent) : LineEditFind(parent)
{
    hideFindButton();
}

void LineEditX::enableOnlyNumberInput()
{
    lineEdit()->setValidator(new QIntValidator(this));
}
