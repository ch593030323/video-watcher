#ifndef MAINSYSTEMSETINGSFORM_H
#define MAINSYSTEMSETINGSFORM_H

#include <QWidget>

namespace Ui {
class MainSystemSetingsForm;
}

class MainSystemSetingsForm : public QWidget
{
    Q_OBJECT

public:
    explicit MainSystemSetingsForm(QWidget *parent = 0);
    ~MainSystemSetingsForm();

public slots:
    void toadd();
    void todel();
    void tomodfy();
private:
    Ui::MainSystemSetingsForm *ui;
};

#endif // MAINSYSTEMSETINGSFORM_H
