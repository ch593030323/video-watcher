#include "mainwindow.h"
#include "ui_mainwindow.h"



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
    lds::init();
    ui->setupUi(this);

    this->setStyleSheet(lds::styleSheetString);
    setObjectName("Window");
    setFocusPolicy(Qt::StrongFocus);

    ui->pushButton_video_widget->setCheckable(true);
    ui->pushButton_video_playback->setCheckable(true);
    ui->pushButton_system_setting->setCheckable(true);
    QButtonGroup *buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(ui->pushButton_video_widget);
    buttonGroup->addButton(ui->pushButton_video_playback);
    buttonGroup->addButton(ui->pushButton_system_setting);
    buttonGroup->setExclusive(true);
    ui->pushButton_video_widget->setChecked(true);

    connect(ui->pushButton_exit, SIGNAL(clicked()), this, SLOT(toexit()));
    connect(ui->pushButton_video_widget, SIGNAL(clicked()), this, SLOT(tovideo_widget()));
    connect(ui->pushButton_video_playback, SIGNAL(clicked()), this, SLOT(tovideo_playback()));
    connect(ui->pushButton_system_setting, SIGNAL(clicked()), this, SLOT(tovideo_system_setting()));
}

MainFrame::~MainFrame()
{
    delete ui;
}


void MainFrame::toexit()
{
    this->close();
    qApp->quit();
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
}
