#ifndef MAINSYSTEMSETINGSALTER_H
#define MAINSYSTEMSETINGSALTER_H

#include <QWidget>

class DataSource;
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

    void setDataSource(DataSource *datasource);

public slots:
    void toadd();
    void todel();
    void tomodify();

private:
    Ui::MainSystemSetingsAlter *ui;
    DataSource *m_datasource;
};

#endif // MAINSYSTEMSETINGSALTER_H
