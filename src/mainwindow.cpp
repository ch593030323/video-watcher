#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_mainvideowidget.h"
#include "jsoncpppath.h"
#include "treevideoview.h"

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

MainFrame::MainFrame(QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::MainWidget)
{
    //
    lds::init(this);

    ui->setupUi(this);

    lds::statusBar = ui->statusBar;

    PlayThread::threadParent = this;

    //
    setStyleSheet(lds::styleSheetString);
    setObjectName("Window");
    setFocusPolicy(Qt::NoFocus);

    this->showMessage("程序启动");

    ui->pushButton_video_widget->setCheckable(true);
    ui->pushButton_video_playback->setCheckable(true);
    ui->pushButton_system_setting->setCheckable(true);
    QButtonGroup *buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(ui->pushButton_video_widget);
    buttonGroup->addButton(ui->pushButton_video_playback);
    buttonGroup->addButton(ui->pushButton_system_setting);
    buttonGroup->setExclusive(true);
    ui->pushButton_video_widget->setChecked(true);

    m_videoWidget = qobject_cast<MainVideoWidget *>(ui->stackedWidget->widget(0));
    m_systemSettings = qobject_cast<MainSystemSetings *>(ui->stackedWidget->widget(2));

    connect(ui->pushButton_exit, SIGNAL(clicked()), this, SLOT(toexit()));
    connect(ui->pushButton_video_widget, SIGNAL(clicked()), this, SLOT(tovideo_widget()));
    connect(ui->pushButton_video_playback, SIGNAL(clicked()), this, SLOT(tovideo_playback()));
    connect(ui->pushButton_system_setting, SIGNAL(clicked()), this, SLOT(tovideo_system_setting()));
    connect(ui->pushButton_system_setting, SIGNAL(clicked()), this, SLOT(tovideo_system_setting()));
    connect(m_videoWidget, SIGNAL(signalStatusbarSetVisiable(bool)), ui->statusBar, SLOT(setVisible(bool)));
    connect(lds::treeSignalTransfer, SIGNAL(signalUpdateUrl()), m_videoWidget, SLOT(updateTreeVisiableItemUrl()));

#ifdef OMS_DATASOURCE
    //由插件程序提供数据源
#else
    setDataSource(new DataSource(this));
#endif

    QTimer::singleShot(0, this, SLOT(loadLayout()));
}

MainFrame::~MainFrame()
{
    delete ui;
}

void MainFrame::setDataSource(DataSource *dataSource)
{
    lds::dataSource = dataSource;
    m_videoWidget->setDataSource(dataSource);
    m_systemSettings->setDataSource(dataSource);
}

void MainFrame::showMessage(const QString &text)
{
    ui->statusBar->showMessage(text);
}

bool MainFrame::isVisiableStatusBar()
{
    return ui->statusBar->isVisible();
}

void MainFrame::saveLayout()
{
    JsonCppPath p(lds::configDirectory + "/start.json");
    p.setValue("/settings/camera_panel_visiable", m_videoWidget->ui->widget_ins_panel->isVisible());
    p.setValue("/settings/window_statusbar_visiable", ui->statusBar->isVisible());
    p.setJsonValue("/layout", m_videoWidget->ui->widget_video->toJsonValue());

    if(!p.saveToFile()) {
        lds::showMessage((p.errorString() + ":" + p.fileName()));
    }
}

void MainFrame::loadLayout()
{
    JsonCppPath p(lds::configDirectory + "/start.json");
    if(!p.parse()) {
        lds::showMessage(("首次启动"));
//        lds::showMessage((p.errorString() + ":" + p.fileName()));
//        return;
    }
    m_videoWidget->ui->widget_video->updateLayout(LayoutInfo::readFrom(p.jsonValue("/layout", LayoutInfo(4).toJsonValue())));

    m_videoWidget->ui->widget_ins_panel->setVisible(p.value("/settings/camera_panel_visiable", false).toBool());
    m_videoWidget->ui->pushButton_control->setChecked(p.value("/settings/camera_panel_visiable", false).toBool());

    ui->statusBar->setVisible(p.value("/settings/window_statusbar_visiable", false).toBool());
    m_videoWidget->ui->pushButton_statusbar->setChecked(p.value("/settings/window_statusbar_visiable", false).toBool());

    //check
    if(1 == m_videoWidget->ui->widget_video->columnCount()) {
        m_videoWidget->ui->pushButton_1x1->setChecked(true);
    } else if(2 == m_videoWidget->ui->widget_video->columnCount()) {
        m_videoWidget->ui->pushButton_2x2->setChecked(true);
    } else if(3 == m_videoWidget->ui->widget_video->columnCount()) {
        m_videoWidget->ui->pushButton_3x3->setChecked(true);
    } else if(4 == m_videoWidget->ui->widget_video->columnCount()) {
        m_videoWidget->ui->pushButton_4x4->setChecked(true);
    }
}

void MainFrame::setVisiableStatusBar(bool showed)
{
    ui->statusBar->setVisible(showed);
}

void MainFrame::toexit()
{
    saveLayout();
    this->close();
}

void MainFrame::tovideo_widget()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainFrame::tovideo_playback()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void MainFrame::tovideo_system_setting()
{
    ui->stackedWidget->setCurrentIndex(2);
    m_systemSettings->refresh();
}
