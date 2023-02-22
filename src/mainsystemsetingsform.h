#ifndef MAINSYSTEMSETINGSFORM_H
#define MAINSYSTEMSETINGSFORM_H

#include <QWidget>
#include <QStandardItemModel>

namespace Ui {
class MainSystemSetingsForm;
}

class MainSystemSetingsForm : public QWidget
{
    Q_OBJECT

public:
    explicit MainSystemSetingsForm(QWidget *parent = 0);
    ~MainSystemSetingsForm();

    void refresh();
public slots:
    void toadd();
    void todel();
    void tomodify();
private:
    Ui::MainSystemSetingsForm *ui;
};

#endif // MAINSYSTEMSETINGSFORM_H
