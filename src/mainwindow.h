#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QStandardItemModel>
#include "videowidget.h"
#include <QtGui>
//#include "QJsonDocument.h"
//#include "QJsonObject.h"
//#include "QJsonArray.h"
#include "ffmpeg.h"


class TreeVideoModel;
QT_BEGIN_NAMESPACE
namespace Ui { class MainWidget; }
QT_END_NAMESPACE

//这里用MainWindow的话，插件编译会有问题，做记录
class MainFrame : public QWidget
{
    Q_OBJECT

public:
    MainFrame(QWidget *parent = 0);
    ~MainFrame();

    void updateLayout();
    void updateCameraTree();
    void updateCameraItemList(const QString &location_obid);
    void updateCameraItemList(QStandardItem *item_location);
    void updateAndExpandNode(const QString &loaction_obid);
private slots:

    /**
     * @brief toVideoLayout1x1
     * 三种video的布局
     */
    void toVideoLayout1x1();
    void toVideoLayout2x2();
    void toVideoLayout3x3();
    void toVideoLayout4x4();

    /**
     * @brief toControlCamera
     * 摄像头的控制，l:left，t:top,r:right,b:bottom
     */
    void toControlCamera_lt();
    void toControlCamera_t();
    void toControlCamera_rt();
    void toControlCamera_l();
    void toControlCamera_c();
    void toControlCamera_r();
    void toControlCamera_lb();
    void toControlCamera_b();
    void toControlCamera_rb();

    void cameraMove(int dx, int dy);
    void cameraMoveRestore();

    //新增组播表单
    void toPlayFormNew();
    void toPlayFormOK();
    //新增轮播表单
    void toPlayAlternateNew();
    void toPlayAlternateOK();
    void toHoldVideoFocus();
    void toReleaseVideoFocus();

    virtual void toSearchStation(int index);
    virtual void toSearchCamera(const QString &string);

protected:
    void resizeEvent(QResizeEvent *event);
protected:
    Ui::MainWidget *ui;
protected:
    TreeVideoModel *m_treeModel;
    QMap<LayoutPos, VideoWidget *> m_videoMap;
    LayoutInfo m_layoutInfo;

};
#endif // MAINWINDOW_H
