#ifndef ICONBUTTON_H
#define ICONBUTTON_H

#include <QWidget>

class IconButton : public QWidget
{
    Q_OBJECT
public:
    enum Type{
        NormalButton,
        ToolButton
    };
public:
    explicit IconButton(QWidget *parent = nullptr);

    void setIconChar(QChar ch);
    void setType(Type type);

signals:
    void clicked();

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);

private:
    QChar ch;
    QColor color;
    Type type;
};

#endif // ICONBUTTON_H
