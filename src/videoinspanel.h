#ifndef VIDEOINSPANEL_H
#define VIDEOINSPANEL_H

#include <QFrame>

namespace Ui {
class VideoInsPanel;
}

class VideoInsPanel : public QFrame
{
    Q_OBJECT

public:
    explicit VideoInsPanel(QWidget *parent = 0);
    ~VideoInsPanel();

    void setStepValue(int value);
protected:
    void resizeEvent(QResizeEvent *event);

private:
    Ui::VideoInsPanel *ui;
};

#endif // VIDEOINSPANEL_H
