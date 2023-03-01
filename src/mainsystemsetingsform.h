#ifndef MAINSYSTEMSETINGSFORM_H
#define MAINSYSTEMSETINGSFORM_H

#include <QWidget>
#include <QStandardItemModel>

class DataSource;
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
    void setDataSource(DataSource *datasource);
public slots:
    void toadd();
    void todel();
    void tomodify();
private:
    Ui::MainSystemSetingsForm *ui;
    DataSource *m_datasource;
};

#endif // MAINSYSTEMSETINGSFORM_H
