#include "mainvideowidget.h"
#include "ui_mainvideowidget.h"
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


MainVideoWidget::MainVideoWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainVideoWidget)
{
    ui->setupUi(this);

    setDataSource(new DataSource(this));
    ui->widget_ins_panel->setStepValue(30);

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

    connect(ui->pushButton_fullscreen, SIGNAL(clicked()), this, SLOT(toVideoShowMax()));
    connect(ui->pushButton_fullscreen_exit, SIGNAL(clicked()), this, SLOT(toVideoShowNormal()));
}

MainVideoWidget::~MainVideoWidget()
{
    delete ui;
}

void MainVideoWidget::resizeEvent(QResizeEvent *event)
{
    //updateLayout();
    QWidget::resizeEvent(event);
}

void MainVideoWidget::updateLayout()
{
    ui->widget_video->updateLayout();
}

void MainVideoWidget::setDataSource(DataSource *datasource)
{
    ui->treeView->setDataSource(datasource);
    ui->treeView->slotInitAll();
}

void MainVideoWidget::toVideoLayout1x1()
{
    ui->widget_video->updateLayout(LayoutInfo(1, 1));
}

void MainVideoWidget::toVideoLayout2x2()
{
    ui->widget_video->updateLayout(LayoutInfo(2, 2));
}

void MainVideoWidget::toVideoLayout3x3()
{
    ui->widget_video->updateLayout(LayoutInfo(3, 3));
}

void MainVideoWidget::toVideoLayout4x4()
{
    ui->widget_video->updateLayout(LayoutInfo(4, 4));
}

void MainVideoWidget::toHoldVideoFocus()
{
    if(!VideoCell::lastFocusWidget)
        return;
    VideoCell::lastFocusWidget->setCheckable(true);
    VideoCell::lastFocusWidget->setChecked(true);
}

void MainVideoWidget::toReleaseVideoFocus()
{
    if(!VideoCell::lastFocusWidget)
        return;
    VideoCell::lastFocusWidget->setCheckable(false);
    VideoCell::lastFocusWidget->setChecked(false);
    VideoCell::lastFocusWidget = NULL;
}

void MainVideoWidget::toVideoShowMax()
{
//    ui->frame_title->hide();
    ui->frame_view->hide();
    this->layout()->update();

    ui->pushButton_fullscreen_exit->show();
    ui->pushButton_fullscreen->hide();
}

void MainVideoWidget::toVideoShowNormal()
{
//    ui->frame_title->show();
    ui->frame_view->show();
    this->layout()->update();

    ui->pushButton_fullscreen_exit->hide();
    ui->pushButton_fullscreen->show();
}
