#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "json/json.h"
#include "treevideomodel.h"
#include "playformnewdialog.h"
#include "playalternatenewdialog.h"
#include "lds.h"
#include "treevideoview.h"
#include "propertycolor.h"

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

MainFrame::MainFrame(QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::MainWidget)
{
    lds::init();
    ui->setupUi(this);

    this->setStyleSheet(lds::styleSheetString);
    setObjectName("Window");
    setFocusPolicy(Qt::StrongFocus);

    ui->frame_play_alternate->setObjectName("Window");
    ui->frame_play_form->setObjectName("Window");

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

    //buttongroup
    ui->pushButton_1x1->setIcon(lds::getLayoutPixmap(1));
    ui->pushButton_1x1->setText("");
    ui->pushButton_1x1->setCheckable(true);
    ui->pushButton_2x2->setIcon(lds::getLayoutPixmap(2));
    ui->pushButton_2x2->setText("");
    ui->pushButton_2x2->setCheckable(true);
    ui->pushButton_3x3->setIcon(lds::getLayoutPixmap(3));
    ui->pushButton_3x3->setText("");
    ui->pushButton_3x3->setCheckable(true);
    ui->pushButton_test->setIcon(lds::getLayoutPixmap(4));
    ui->pushButton_test->setText("");
    ui->pushButton_test->setCheckable(true);

    QButtonGroup *buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(ui->pushButton_1x1);
    buttonGroup->addButton(ui->pushButton_2x2);
    buttonGroup->addButton(ui->pushButton_3x3);
    buttonGroup->addButton(ui->pushButton_test);
    buttonGroup->setExclusive(true);

    //checked
    ui->widget_video->updateLayout(LayoutInfo(3, 3));
    ui->pushButton_3x3->setChecked(true);

    ui->pushButton_fullscreen->setIcon(lds::getFontPixmap(0xf424, PropertyColor::buttonTextColor, QSize(15, 15)));
    ui->pushButton_fullscreen_exit->setIcon(lds::getFontPixmap(0xf422, PropertyColor::buttonTextColor, QSize(15, 15)));
    ui->pushButton_fullscreen_exit->hide();

    //connect
    connect(ui->pushButton_1x1, SIGNAL(clicked()), this, SLOT(toVideoLayout1x1()));
    connect(ui->pushButton_2x2, SIGNAL(clicked()), this, SLOT(toVideoLayout2x2()));
    connect(ui->pushButton_3x3, SIGNAL(clicked()), this, SLOT(toVideoLayout3x3()));
    connect(ui->pushButton_test, SIGNAL(clicked()), this, SLOT(toVideoLayout4x4()));

    connect(ui->pushButton_form_new, SIGNAL(clicked()), this, SLOT(toPlayFormNew()));
    connect(ui->pushButton_form_ok, SIGNAL(clicked()), this, SLOT(toPlayFormOK()));
    connect(ui->pushButton_alternate_new, SIGNAL(clicked()), this, SLOT(toPlayAlternateNew()));
    connect(ui->pushButton_alternate_ok, SIGNAL(clicked()), this, SLOT(toPlayAlternateOK()));
    connect(ui->pushButton_play_alternate->menu(), SIGNAL(aboutToShow()), this, SLOT(toHoldVideoFocus()));
    connect(ui->pushButton_play_alternate->menu(), SIGNAL(aboutToHide()), this, SLOT(toReleaseVideoFocus()));
    connect(ui->pushButton_exit, SIGNAL(clicked()), this, SLOT(toexit()));
    connect(ui->pushButton_fullscreen, SIGNAL(clicked()), this, SLOT(toVideoShowMax()));
    connect(ui->pushButton_fullscreen_exit, SIGNAL(clicked()), this, SLOT(toVideoShowNormal()));
}

MainFrame::~MainFrame()
{
    delete ui;
}

void MainFrame::resizeEvent(QResizeEvent *event)
{
    //updateLayout();
    QWidget::resizeEvent(event);
}

void MainFrame::updateLayout()
{
    ui->widget_video->updateLayout();
}

void MainFrame::setDataSource(DataSource *datasource)
{
    ui->treeView->setDataSource(datasource);
    ui->treeView->slotInitAll();
}

void MainFrame::toVideoLayout1x1()
{
    ui->widget_video->updateLayout(LayoutInfo(1, 1));
}

void MainFrame::toVideoLayout2x2()
{
    ui->widget_video->updateLayout(LayoutInfo(2, 2));
}

void MainFrame::toVideoLayout3x3()
{
    ui->widget_video->updateLayout(LayoutInfo(3, 3));
}

void MainFrame::toVideoLayout4x4()
{
    ui->widget_video->updateLayout(LayoutInfo(4, 4));
}

void MainFrame::toPlayFormNew()
{
    playformnewdialog dialog(this);
    dialog.setDataSource(ui->treeView->dataSource());
    dialog.resize(this->size());
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
    LayoutInfo info =  LayoutInfo::readFrom(file.readAll());
    ui->widget_video->updateLayout(info);
}

void MainFrame::toPlayAlternateNew()
{
    PlayAlternateNewDialog dialog(this);
    dialog.setDataSource(ui->treeView->dataSource());
    dialog.resize(this->size());
    dialog.exec();
}

void MainFrame::toPlayAlternateOK()
{
    if(!VideoCell::lastFocusWidget)
        return;
    int row = ui->comboBox_alternate->currentIndex();
    if(row < 0)
        return;

    VideoCell::lastFocusWidget->addPlayerList(AlterPlayFrame::readFrom(ui->comboBox_alternate->itemData(row).toString()));
}

void MainFrame::toHoldVideoFocus()
{
    if(!VideoCell::lastFocusWidget)
        return;
    VideoCell::lastFocusWidget->setCheckable(true);
    VideoCell::lastFocusWidget->setChecked(true);
}

void MainFrame::toReleaseVideoFocus()
{
    if(!VideoCell::lastFocusWidget)
        return;
    VideoCell::lastFocusWidget->setCheckable(false);
    VideoCell::lastFocusWidget->setChecked(false);
    VideoCell::lastFocusWidget = NULL;
}

void MainFrame::toexit()
{
    this->close();
    qApp->quit();
}

void MainFrame::toVideoShowMax()
{
    ui->frame_title->hide();
    ui->treeView->hide();
    this->layout()->update();

    ui->pushButton_fullscreen_exit->show();
    ui->pushButton_fullscreen->hide();
}

void MainFrame::toVideoShowNormal()
{
    ui->frame_title->show();
    ui->treeView->show();
    this->layout()->update();

    ui->pushButton_fullscreen_exit->hide();
    ui->pushButton_fullscreen->show();
}
