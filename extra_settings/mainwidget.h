#ifndef MAINWIDGET_H
#define MAINWIDGET_H
#include "src/datasource.h"

#include <QFrame>
#include <QStandardItemModel>
#include <QAbstractItemDelegate>
#include <QSettings>

namespace Ui {
class mainWidget;
}

class QAbstractButton;

class mainWidget : public QFrame
{
    Q_OBJECT

public:
    explicit mainWidget(QWidget *parent = 0);
    ~mainWidget();

private slots:
    void toexit();
    void toreload();
    void toSwitchPage(QAbstractButton *button, bool checked);
    void tosave();

private:
    Ui::mainWidget *ui;
    DataSource *m_datasource;
    QSettings *conf;
};

#endif // MAINWIDGET_H
