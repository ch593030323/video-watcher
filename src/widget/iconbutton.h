#ifndef ICONBUTTON_H
#define ICONBUTTON_H

#include <QWidget>

class IconButton : public QWidget
{
    Q_OBJECT
public:
    explicit IconButton(QWidget *parent = nullptr);
    void setIconChar(QChar ch);
protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
private:
    QChar ch;
    QColor color;

};

#endif // ICONBUTTON_H
