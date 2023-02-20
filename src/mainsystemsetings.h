#ifndef MAINSYSTEMSETINGS_H
#define MAINSYSTEMSETINGS_H

#include <QWidget>

namespace Ui {
class MainSystemSetings;
}

class MainSystemSetings : public QWidget
{
    Q_OBJECT

public:
    explicit MainSystemSetings(QWidget *parent = 0);
    ~MainSystemSetings();

private slots:
    //新增组播表单
    void toPlayFormNew();
    void toPlayFormOK();
    //新增轮播表单
    void toPlayAlternateNew();
    void toPlayAlternateOK();

    void toalter();
    void toform();
private:
    Ui::MainSystemSetings *ui;
};

#endif // MAINSYSTEMSETINGS_H
