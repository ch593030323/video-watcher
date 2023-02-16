#ifndef LINEEDITX_H
#define LINEEDITX_H

#include "lineeditfind.h"

class LineEditX : public LineEditFind
{
    Q_OBJECT
public:
    LineEditX(QWidget *parent = 0);

    void enableOnlyNumberInput();
};

#endif // LINEEDITX_H
