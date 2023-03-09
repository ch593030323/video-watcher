#ifndef AUTOCLOSELABEL_H
#define AUTOCLOSELABEL_H

#include <QLabel>

class AutoCloseLabel : public QLabel
{
    Q_OBJECT
public:
    AutoCloseLabel(QWidget *parent = 0);
    ~AutoCloseLabel();

public:
    void mousePressEvent(QMouseEvent *);
};

#endif // AUTOCLOSELABEL_H
