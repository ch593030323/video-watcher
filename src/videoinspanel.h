#ifndef VIDEOINSPANEL_H
#define VIDEOINSPANEL_H

#include <QWidget>

namespace Ui {
class VideoInsPanel;
}

class VideoInsPanel : public QWidget
{
    Q_OBJECT

public:
    explicit VideoInsPanel(QWidget *parent = 0);
    ~VideoInsPanel();

private:
    Ui::VideoInsPanel *ui;
};

#endif // VIDEOINSPANEL_H
