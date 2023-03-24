#ifndef MAINSYSTEMSETINGS_H
#define MAINSYSTEMSETINGS_H

#include <QWidget>
#include <QPushButton>


class MainSystemSetingsAlter;
class MainSystemSetingsForm;
class DataSource;
class MainSystemSetingsSnap;
class MainSystemSetingsUrl;

namespace Ui {
class MainSystemSetings;
}

class MainSystemSetings : public QWidget
{
    Q_OBJECT

public:
    static void setFlat(QPushButton *button, const QPixmap &pixmap);

public:
    explicit MainSystemSetings(QWidget *parent = 0);
    ~MainSystemSetings();

    void setDataSource(DataSource *datasource);
    void refresh();
signals:
    void signalBack();
private slots:
    void toalter();
    void toform();
    void tosnap();
    void tourl();
private:
    Ui::MainSystemSetings *ui;
    DataSource *m_datasource;

    MainSystemSetingsAlter *m_settingsAlter;
    MainSystemSetingsForm *m_settingsForm;
    MainSystemSetingsSnap *m_settingsSnap;
    MainSystemSetingsUrl *m_settingsUrl;
};

#endif // MAINSYSTEMSETINGS_H
