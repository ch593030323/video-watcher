#ifndef MAINWIDGET_H
#define MAINWIDGET_H
#include "src/datasource.h"
#include "jsoncpppath.h"
#include "editedwidgetmapper.h"

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
    bool saveJson();
private:
    Ui::mainWidget *ui;
    DataSource *m_datasource;
    JsonCppPath *m_config;
    EditedWidgetMapper *m_mapper;
};

#endif // MAINWIDGET_H
