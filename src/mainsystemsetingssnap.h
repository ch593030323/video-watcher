#ifndef MAINSYSTEMSETINGSSNAP_H
#define MAINSYSTEMSETINGSSNAP_H

#include <QWidget>

namespace Ui {
class MainSystemSetingsSnap;
}
class QStandardItemModel;
class MainSystemSetingsSnap : public QWidget
{
    Q_OBJECT

public:
    explicit MainSystemSetingsSnap(QWidget *parent = 0);
    ~MainSystemSetingsSnap();


    void refresh();

private slots:
    void toShowMax(const QModelIndex &index);
    void toDeleteCurrentSnap();

    void toRefresh();
private:
    Ui::MainSystemSetingsSnap *ui;
    QStandardItemModel *model;
};

#endif // MAINSYSTEMSETINGSSNAP_H
