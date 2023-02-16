#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "videocell.h"
#include "ffmpeg.h"
#include "datasource.h"

#include <QWidget>
#include <QStandardItemModel>
#include <QtGui>
//#include "QJsonDocument.h"
//#include "QJsonObject.h"
//#include "QJsonArray.h"


class TreeVideoModel;
QT_BEGIN_NAMESPACE
namespace Ui { class MainWidget; }
QT_END_NAMESPACE

//这里用MainWindow的话，插件编译会有问题，做记录
class MainFrame : public QFrame
{
    Q_OBJECT
public:
    MainFrame(QWidget *parent = 0);
    ~MainFrame();

    void updateLayout();
    void setDataSource(DataSource *datasource);

private slots:

    /**
     * @brief toVideoLayout1x1
     * 三种video的布局
     */
    void toVideoLayout1x1();
    void toVideoLayout2x2();
    void toVideoLayout3x3();
    void toVideoLayout4x4();

    //新增组播表单
    void toPlayFormNew();
    void toPlayFormOK();
    //新增轮播表单
    void toPlayAlternateNew();
    void toPlayAlternateOK();
    void toHoldVideoFocus();
    void toReleaseVideoFocus();

    void toexit();

    void toVideoShowMax();
    void toVideoShowNormal();

protected:
    void resizeEvent(QResizeEvent *event);

protected:
    Ui::MainWidget *ui;
};
#endif // MAINWINDOW_H
