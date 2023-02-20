#ifndef MAINSYSTEMSETINGSALTER_H
#define MAINSYSTEMSETINGSALTER_H

#include <QWidget>

namespace Ui {
class MainSystemSetingsAlter;
}

class MainSystemSetingsAlter : public QWidget
{
    Q_OBJECT

public:
    explicit MainSystemSetingsAlter(QWidget *parent = 0);
    ~MainSystemSetingsAlter();

private:
    Ui::MainSystemSetingsAlter *ui;
};

#endif // MAINSYSTEMSETINGSALTER_H
