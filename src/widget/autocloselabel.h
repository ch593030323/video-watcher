#ifndef AUTOCLOSELABEL_H
#define AUTOCLOSELABEL_H

#include <QLabel>

class AutoCloseLabel : public QLabel
{
    Q_OBJECT
public:
    AutoCloseLabel(QWidget *parent = 0);
    ~AutoCloseLabel();

protected:
    void mousePressEvent(QMouseEvent *e);
    void hideEvent(QHideEvent *e);

};

#endif // AUTOCLOSELABEL_H
