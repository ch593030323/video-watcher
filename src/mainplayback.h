#ifndef MAINPLAYBACK_H
#define MAINPLAYBACK_H

#include <QWidget>

namespace Ui {
class MainPlayBack;
}

class MainPlayBack : public QWidget
{
    Q_OBJECT

public:
    explicit MainPlayBack(QWidget *parent = 0);
    ~MainPlayBack();

private:
    Ui::MainPlayBack *ui;
};

#endif // MAINPLAYBACK_H
