#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "json/json.h"
#include "treevideomodel.h"
#include "playformnewdialog.h"
#include "playalternatenewdialog.h"
#include "lds.h"

#include <QTreeView>
#include <QStandardItemModel>
#include <QtDebug>
#include <QWidgetAction>


using namespace std;
/*

http://vfx.mtime.cn/Video/2019/02/04/mp4/190204084208765161.mp4
http://vfx.mtime.cn/Video/2019/03/19/mp4/190319212559089721.mp4
http://vfx.mtime.cn/Video/2019/03/17/mp4/190317150237409904.mp4
http://vfx.mtime.cn/Video/2019/03/14/mp4/190314223540373995.mp4

4:28
http://vfx.mtime.cn/Video/2021/01/07/mp4/210107172407759182_1080.mp4

1:43
http://vfx.mtime.cn/Video/2019/04/02/mp4/190402084548910831.mp4

*/
static QString cameraStateString(QString state) {
    if("0" == state)
        return QString::fromUtf8("在线");
    if("1" == state)
        return QString::fromUtf8("故障");
    if("2" == state)
        return QString::fromUtf8("离线");
    return "unknow code:" + state;
}

MainFrame::MainFrame(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget)
{
    ui->setupUi(this);

    lds::init();
    this->setStyleSheet(lds::styleSheetString);
    setObjectName("Window");
    setFocusPolicy(Qt::StrongFocus);

    ui->frame_play_alternate->setObjectName("Window");
    ui->frame_play_form->setObjectName("Window");
    ui->widget_cameraIns->layout()->setMargin(0);
    ui->widget_cameraIns->hide();

    m_treeModel = new TreeVideoModel(this);
    ui->treeView->setModel(m_treeModel);
    ui->treeView->setHeaderHidden(true);
    ui->treeView->header()->setStretchLastSection(true);
    ui->treeView->expandAll();
    ui->treeView->setDragEnabled(true);
    ui->treeView->setEditTriggers(QTreeView::NoEditTriggers);

    //VideoWidget
    m_layoutInfo = LayoutInfo(3, 3);
    //
    {
        ui->frame_play_form->setFixedSize(500, 60);
        ui->frame_play_form->layout()->setMargin(10);
        QWidgetAction *wac = new QWidgetAction( ui->pushButton_play_form);
        wac->setDefaultWidget(ui->frame_play_form);
        QMenu *menu_form = new QMenu(ui->pushButton_play_form);
        menu_form->addAction(wac);
        ui->pushButton_play_form->setMenu(menu_form);
    }
    {
        ui->frame_play_alternate->setFixedSize(500, 60);
        ui->frame_play_alternate->layout()->setMargin(10);
        QWidgetAction *wac = new QWidgetAction( ui->pushButton_play_alternate);
        wac->setDefaultWidget(ui->frame_play_alternate);
        QMenu *menu_alternate = new QMenu(ui->pushButton_play_alternate);
        menu_alternate->addAction(wac);
        ui->pushButton_play_alternate->setMenu(menu_alternate);
    }

    ui->pushButton_1x1->setIcon(lds::getLayoutPixmap(1));
    ui->pushButton_1x1->setText("");
    ui->pushButton_2x2->setIcon(lds::getLayoutPixmap(2));
    ui->pushButton_2x2->setText("");
    ui->pushButton_3x3->setIcon(lds::getLayoutPixmap(3));
    ui->pushButton_3x3->setText("");
    ui->pushButton_test->setIcon(lds::getLayoutPixmap(4));
    ui->pushButton_test->setText("");
    //connect
    connect(ui->pushButton_1x1, SIGNAL(clicked()), this, SLOT(toVideoLayout1x1()));
    connect(ui->pushButton_2x2, SIGNAL(clicked()), this, SLOT(toVideoLayout2x2()));
    connect(ui->pushButton_3x3, SIGNAL(clicked()), this, SLOT(toVideoLayout3x3()));
    connect(ui->pushButton_test, SIGNAL(clicked()), this, SLOT(toVideoLayout4x4()));

    connect(ui->pushButton_cameraIns_lt, SIGNAL(clicked()), this, SLOT(toControlCamera_lt()));
    connect(ui->pushButton_cameraIns_t, SIGNAL(clicked()), this, SLOT(toControlCamera_t()));
    connect(ui->pushButton_cameraIns_rt, SIGNAL(clicked()), this, SLOT(toControlCamera_rt()));

    connect(ui->pushButton_cameraIns_l, SIGNAL(clicked()), this, SLOT(toControlCamera_l()));
    connect(ui->pushButton_cameraIns_c, SIGNAL(clicked()), this, SLOT(toControlCamera_c()));
    connect(ui->pushButton_cameraIns_r, SIGNAL(clicked()), this, SLOT(toControlCamera_r()));

    connect(ui->pushButton_cameraIns_lb, SIGNAL(clicked()), this, SLOT(toControlCamera_lb()));
    connect(ui->pushButton_cameraIns_b, SIGNAL(clicked()), this, SLOT(toControlCamera_b()));
    connect(ui->pushButton_cameraIns_rb, SIGNAL(clicked()), this, SLOT(toControlCamera_rb()));

    connect(ui->pushButton_form_new, SIGNAL(clicked()), this, SLOT(toPlayFormNew()));
    connect(ui->pushButton_form_ok, SIGNAL(clicked()), this, SLOT(toPlayFormOK()));
    connect(ui->pushButton_alternate_new, SIGNAL(clicked()), this, SLOT(toPlayAlternateNew()));
    connect(ui->pushButton_alternate_ok, SIGNAL(clicked()), this, SLOT(toPlayAlternateOK()));
    connect(ui->pushButton_play_alternate->menu(), SIGNAL(aboutToShow()), this, SLOT(toHoldVideoFocus()));
    connect(ui->pushButton_play_alternate->menu(), SIGNAL(aboutToHide()), this, SLOT(toReleaseVideoFocus()));
    //play_form
    {
        QDirIterator dir("play_form");
        while(dir.hasNext()) {
            dir.next();
            if(dir.fileInfo().isFile()) {
                ui->comboBox_form->addItem(dir.fileName(), dir.filePath());
            }
        }
    }

    //play_alter
    {
        QDirIterator dir("play_alter");
        while(dir.hasNext()) {
            dir.next();
            if(dir.fileInfo().isFile()) {
                ui->comboBox_alternate->addItem(dir.fileName(), dir.filePath());
            }
        }
    }

//#ifdef Q_OS_WIN
    updateCameraTree();
    ui->treeView->expandAll();
//#endif
}

MainFrame::~MainFrame()
{
    delete ui;
}

void MainFrame::resizeEvent(QResizeEvent *event)
{
    updateLayout();
    QWidget::resizeEvent(event);
}

void MainFrame::updateLayout()
{
    int videoAreaWidth = this->width() - lds::videoAreaRight - lds::margin;
    int videoAreaHeight = this->height() - lds::videoAreaBottom - lds::margin;
    int treeHeight = videoAreaHeight / 2 - lds::margin;
    int treeLeft = this->width() - lds::videoAreaRight;
    int treeWidth = lds::videoAreaRight - lds::margin;
    //treeView
    ui->comboBox_search_station->setGeometry(
                treeLeft, lds::margin, treeWidth * 0.3,  30);
    ui->label_find->setGeometry(
                ui->comboBox_search_station->geometry().right(), lds::margin, 30,  30);
    ui->lineEdit_camera->setGeometry(
                ui->label_find->geometry().right(), lds::margin, treeWidth * 0.7 - 30 + lds::margin / 2,  30);
    ui->treeView->setGeometry(
                treeLeft, lds::margin + 1 * (30 + 10), treeWidth,  treeHeight);
    //widget_cameraIns
    ui->widget_cameraIns->setGeometry(treeLeft, lds::marginX2 + treeHeight, treeWidth, treeWidth);

    //按钮布局
    int buttonTop = this->height() - lds::videoAreaBottom;// + (lds::videoAreaBottom - b40) / 2;
    ui->pushButton_1x1->setGeometry(lds::margin + 0,    buttonTop, 80, b40);
    ui->pushButton_2x2->setGeometry(lds::margin + 100,  buttonTop, 80, b40);
    ui->pushButton_3x3->setGeometry(lds::margin + 200,  buttonTop, 80, b40);
    ui->pushButton_test->setGeometry(lds::margin + 300, buttonTop, 80, b40);
    ui->pushButton_play_form->setGeometry(lds::margin + 400, buttonTop, 80, b40);
    ui->pushButton_play_alternate->setGeometry(lds::margin + 500, buttonTop, 80, b40);
    //videoWidget
    VideoWidget::parseVideoArea(m_layoutInfo, this, QRect(lds::margin / 2, lds::margin / 2 + lds::border_width, videoAreaWidth, videoAreaHeight), m_videoMap);
}

void MainFrame::updateCameraTree()
{
    m_treeModel->clear();

    QStandardItem *itemRoot =  m_treeModel->invisibleRootItem();
    QStandardItem *itemISCS = new QStandardItem(QString::fromUtf8("摄像头"));
    itemRoot->appendRow(itemISCS);
    QSqlQuery query_location;
    query_location.exec("select obid, name from location");
    while(query_location.next()) {
        QStandardItem *item_location = new QStandardItem;
        QString location_obid = query_location.record().value("obid").toString();
        QString location_name = query_location.record().value("name").toString();
        item_location->setText(location_name);
        item_location->setData(VideoNodeStation, VideoNodeType);
        item_location->setData(location_obid,    VideoObidRole);
        itemISCS->appendRow(item_location);

        updateCameraNode(item_location);
    }
}

void MainFrame::updateCameraNode(const QString &location_obid)
{
    QStandardItem *itemRoot =  m_treeModel->invisibleRootItem();
    QStandardItem *item =  itemRoot->child(0);
    QStandardItem *item_location = 0;
    for(int k = 0; k < item->rowCount(); k ++) {
        item_location = item->child(k);
        if(item_location && item_location->data(VideoObidRole).toString() == location_obid) {
            break;
        }
    }
    if(!item_location)
        return;
    updateCameraNode(item_location);
}

void MainFrame::updateCameraNode(QStandardItem *item_location)
{
    item_location->removeRows(0, item_location->rowCount());
    QString location_obid = item_location->data(VideoObidRole).toString();
    QSqlQuery query_device;

    query_device.exec(QString("select * from device where location_obid = '%1' ").arg(location_obid));
    while(query_device.next()) {
        QString state = query_device.record().value("state").toString();
        QString name = QString::fromUtf8(query_device.record().value("name").toByteArray());
        QString type = query_device.record().value("type").toString();
        QString obid = query_device.record().value("obid").toString();
        QString url = query_device.record().value("url").toString();

        QStandardItem *item_device = new QStandardItem;
        item_device->setText(name);
        item_device->setToolTip("[" + cameraStateString(state) + "]" + name);
        item_device->setData(VideoNodeTrain,    VideoNodeType);
        item_device->setData(name,              VideoNameRole);
        item_device->setData(type,              VideoTypeRole);
        item_device->setData(obid,              VideoObidRole);
        item_device->setData(state,             VideoStateRole);
        item_device->setData(url,               VideoUrlRole);
        item_device->setData(lds::getFontPixmap(0xf05e, QColor("red"), QSize(12, 12)),   Qt::DecorationRole);
        item_device->setData(lds::getFontPixmap(0x25cf, QColor("green"), QSize(12, 12)),   Qt::DecorationRole);
//            item_device->setData(lds::getFontPixmap(0x25cf, QColor(245, 180, 0), QSize(12, 12)),   Qt::DecorationRole);

        item_location->appendRow(item_device);

        //
        if(!lds::PlayThreadMap.contains(obid)) {
            PlayThread *thread = new PlayThread(url, this);
            lds::PlayThreadMap.insert(obid,thread);
        }
    }
}

void MainFrame::updateAndExpandNode(const QString &loaction_obid)
{

}

void MainFrame::toVideoLayout1x1()
{
    m_layoutInfo = LayoutInfo(1, 1);
    updateLayout();

}

void MainFrame::toVideoLayout2x2()
{
    m_layoutInfo = LayoutInfo(2, 2);
    updateLayout();
}

void MainFrame::toVideoLayout3x3()
{
    m_layoutInfo = LayoutInfo(3, 3);
    updateLayout();
}

void MainFrame::toVideoLayout4x4()
{
    m_layoutInfo = LayoutInfo(4, 4);
    updateLayout();
}

void MainFrame::toControlCamera_lt()
{
    cameraMove(-1, -1);
}

void MainFrame::toControlCamera_t()
{
    cameraMove(0, -1);
}

void MainFrame::toControlCamera_rt()
{
    cameraMove(1, -1);
}

void MainFrame::toControlCamera_l()
{
    cameraMove(-1, 0);
}

void MainFrame::toControlCamera_c()
{
    cameraMoveRestore();
}

void MainFrame::toControlCamera_r()
{
    cameraMove(1, 0);
}

void MainFrame::toControlCamera_lb()
{
    cameraMove(-1, 1);
}

void MainFrame::toControlCamera_b()
{
    cameraMove(0, 1);
}

void MainFrame::toControlCamera_rb()
{
    cameraMove(1, 1);
}

void MainFrame::cameraMove(int dx, int dy)
{
    VideoWidget *w = qobject_cast<VideoWidget *>(this->focusWidget());
    if(!w)
        return;
    //    w->move(dx, dy);
}

void MainFrame::cameraMoveRestore()
{
    VideoWidget *w = qobject_cast<VideoWidget *>(this->focusWidget());
    if(!w)
        return;
    //    w->moveRestore();
}

void MainFrame::toPlayFormNew()
{
    playformnewdialog dialog(this);
    dialog.resize(this->size());
    dialog.setTreeModel(m_treeModel);
    dialog.exec();
}

void MainFrame::toPlayFormOK()
{
    int row = ui->comboBox_form->currentIndex();
    if(row < 0)
        return;
    QFile file(ui->comboBox_form->itemData(row).toString());
    if(!file.open(QFile::ReadOnly)) {
        qDebug() << file.errorString();
    }
    m_layoutInfo = LayoutInfo::readFrom(file.readAll());
    updateLayout();
}

void MainFrame::toPlayAlternateNew()
{
    PlayAlternateNewDialog dialog(this);
    dialog.resize(this->size());
    dialog.setTreeModel(m_treeModel);
    dialog.exec();
}

void MainFrame::toPlayAlternateOK()
{
    if(!VideoWidget::lastFocusWidget)
        return;
    int row = ui->comboBox_alternate->currentIndex();
    if(row < 0)
        return;

    VideoWidget::lastFocusWidget->addPlayerList(AlterPlayFrame::readFrom(ui->comboBox_alternate->itemData(row).toString()));
}

void MainFrame::toHoldVideoFocus()
{
    if(!VideoWidget::lastFocusWidget)
        return;
    VideoWidget::lastFocusWidget->setCheckable(true);
    VideoWidget::lastFocusWidget->setChecked(true);
}

void MainFrame::toReleaseVideoFocus()
{
    if(!VideoWidget::lastFocusWidget)
        return;
    VideoWidget::lastFocusWidget->setCheckable(false);
    VideoWidget::lastFocusWidget->setChecked(false);
    VideoWidget::lastFocusWidget = NULL;
}
