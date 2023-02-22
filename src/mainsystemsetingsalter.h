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
    void refresh();

public slots:
    void toadd();
    void todel();
    void tomodify();

private:
    Ui::MainSystemSetingsAlter *ui;
};

#endif // MAINSYSTEMSETINGSALTER_H
