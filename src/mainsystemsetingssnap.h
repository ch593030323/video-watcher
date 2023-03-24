#ifndef MAINSYSTEMSETINGSSNAP_H
#define MAINSYSTEMSETINGSSNAP_H

#include <QWidget>
#include <QModelIndex>
#include <QMenu>
#include <QLabel>

namespace Ui {
class MainSystemSetingsSnap;
}
class QStandardItemModel;
class TreeVideoViewSearch;
class MainSystemSetingsSnap : public QWidget
{
    Q_OBJECT

public:
    explicit MainSystemSetingsSnap(QWidget *parent = 0);
    ~MainSystemSetingsSnap();


    void refresh();
    void refresh(const QString &obid);
    void refresh(const QModelIndex &index);

private slots:
    void slotShowMax(const QModelIndex &index);
    void slotDeleteCurrentSnap();

    void slotRefresh();

    void slotUpdateDevice();

    void slotSearchSnap(const QModelIndex &index);

private:
    Ui::MainSystemSetingsSnap *ui;
    QStandardItemModel *model;
    TreeVideoViewSearch *m_searchView;
    QMenu *m_menu;
    QLabel *m_noImageLabel;

};

#endif // MAINSYSTEMSETINGSSNAP_H
