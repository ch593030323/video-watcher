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

    ui->pushButton_1x1->setIcon(lds::getLayoutPixmap(1));
    ui->pushButton_1x1->setText("");
    ui->pushButton_2x2->setIcon(lds::getLayoutPixmap(2));
    ui->pushButton_2x2->setText("");
    ui->pushButton_3x3->setIcon(lds::getLayoutPixmap(3));
    ui->pushButton_3x3->setText("");
    ui->pushButton_4x4->setIcon(lds::getLayoutPixmap(4));
    ui->pushButton_4x4->setText("");

    ui->pushButton_more->setIcon(lds::getFontPixmap(0xf141));
    ui->pushButton_more->setText("");
    {
        QMenu *menu = new QMenu(ui->pushButton_more);
        ui->pushButton_more->setMenu(menu);
        connect(menu, SIGNAL(aboutToShow()), this, SLOT(toUpdateMoreMenu()));
    }

    //buttongroup
    ui->pushButton_1x1->setCheckable(true);
    ui->pushButton_2x2->setCheckable(true);
    ui->pushButton_3x3->setCheckable(true);
    ui->pushButton_4x4->setCheckable(true);

    QButtonGroup *buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(ui->pushButton_1x1);
    buttonGroup->addButton(ui->pushButton_2x2);
    buttonGroup->addButton(ui->pushButton_3x3);
    buttonGroup->addButton(ui->pushButton_4x4);
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
    connect(ui->pushButton_4x4, SIGNAL(clicked()), this, SLOT(toVideoLayout4x4()));

    connect(ui->pushButton_fullscreen, SIGNAL(clicked()), this, SLOT(toVideoShowMax()));
    connect(ui->pushButton_fullscreen_exit, SIGNAL(clicked()), this, SLOT(toVideoShowNormal()));

    connect(ui->treeView->view(), SIGNAL(signalPlayRequest(QString)), ui->widget_video, SLOT(slotAddUrlToFocusedCell(QString)));
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

void MainVideoWidget::toUpdateMoreMenu()
{
    QMenu *m = ui->pushButton_more->menu();
    m->clear();
    for(const QFileInfo &info : QDir("play_form").entryInfoList()) {
        if(!info.isFile())
            continue;
        QAction *ac = m->addAction(info.baseName(), this, SLOT(toApplyLayoutInfo()));
        ac->setData(info.filePath());
    }
}

void MainVideoWidget::toApplyLayoutInfo()
{
    QAction *ac = qobject_cast<QAction *>(sender());
    if(!ac)
        return;
    QFile file(ac->data().toString());
    if(!file.open(QFile::ReadOnly)) {
        qDebug() << file.errorString();
        return;
    }
    ui->widget_video->updateLayout(LayoutInfo::readFrom(file.readAll()));
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
