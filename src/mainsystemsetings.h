#ifndef MAINSYSTEMSETINGS_H
#define MAINSYSTEMSETINGS_H

#include <QWidget>


class MainSystemSetingsAlter;
class MainSystemSetingsForm;
class DataSource;
namespace Ui {
class MainSystemSetings;
}

class MainSystemSetings : public QWidget
{
    Q_OBJECT

public:
    explicit MainSystemSetings(QWidget *parent = 0);
    ~MainSystemSetings();

    void setDataSource(DataSource *datasource);
private slots:
    void toalter();
    void toform();
private:
    Ui::MainSystemSetings *ui;
    DataSource *m_datasource;

    MainSystemSetingsAlter *m_settingsAlter;
    MainSystemSetingsForm *m_settingsForm;
};

#endif // MAINSYSTEMSETINGS_H
