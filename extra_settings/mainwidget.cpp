#include "mainwidget.h"
#include "ui_mainwidget.h"
#include "json/json.h"
#include "src/treevideoviewsearchurl.h"
#include "src/treevideomodel.h"
#include "src/treevideoview.h"

#include <QFile>
#include <QtDebug>
#include <QTreeView>
#include <QItemSelectionModel>
#include <QButtonGroup>


mainWidget::mainWidget(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::mainWidget)
{
    ui->setupUi(this);
    this->setObjectName("Window");
    ui->lineEdit_default_rowcount->enableOnlyNumberInput();
    ui->lineEdit_default_columncount->enableOnlyNumberInput();

    conf = new QSettings("setting.ini", QSettings::IniFormat, this);
    ui->lineEdit_default_rowcount->setText(conf->value("global/default_rowcount", 2).toString());
    ui->lineEdit_default_columncount->setText(conf->value("global/default_columncount", 2).toString());
    ui->tableView_defatul_playlist->setStringList(conf->value("global/default_playlist", QStringList()
                                                          <<"http://vfx.mtime.cn/Video/2019/03/14/mp4/190314223540373995.mp4"
                                                          <<"http://vfx.mtime.cn/Video/2021/01/07/mp4/210107172407759182_1080.mp4"
                                                          <<"http://vfx.mtime.cn/Video/2019/03/19/mp4/190319212559089721.mp4"
                                                          <<"http://vfx.mtime.cn/Video/2019/03/17/mp4/190317150237409904.mp")
                                                  .toStringList());

    m_datasource = new DataSource(this);
    //ui->treeView->hideMenu();
    ui->treeView->setDataSource(m_datasource);

    //buttongroup
    ui->toolButton_basic->setCheckable(true);
    ui->toolButton_camera->setCheckable(true);
    QButtonGroup *buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(ui->toolButton_basic);
    buttonGroup->addButton(ui->toolButton_camera);
    buttonGroup->setExclusive(true);
    ui->toolButton_basic->setChecked(true);
    ui->stackedWidget->setCurrentIndex(0);


    connect(ui->pushButton_exit, SIGNAL(clicked()), this, SLOT(toexit()));
    connect(buttonGroup, SIGNAL(buttonToggled(QAbstractButton *, bool)), this, SLOT(toSwitchPage(QAbstractButton *, bool)));
    connect(ui->pushButton_add_defatult_video_url, SIGNAL(clicked()), ui->tableView_defatul_playlist, SLOT(toAdd()));
    connect(ui->pushButton_sub_defatult_video_url, SIGNAL(clicked()), ui->tableView_defatul_playlist, SLOT(toRemove()));

    toreload();
}

mainWidget::~mainWidget()
{
    delete ui;
}

void mainWidget::toexit()
{
    this->close();
}

void mainWidget::toreload()
{
    ui->treeView->slotInitAll();
}

void mainWidget::toSwitchPage(QAbstractButton *button, bool checked)
{
    if(!checked)
        return;
    if(button == ui->toolButton_basic) {
        ui->stackedWidget->setCurrentIndex(0);
    }
    if(button == ui->toolButton_camera) {
        ui->stackedWidget->setCurrentIndex(1);
    }
}

void mainWidget::tosave()
{
    conf->setValue("global/default_rowcount", ui->lineEdit_default_rowcount->text());
    conf->setValue("global/default_columncount", ui->lineEdit_default_columncount->text());
    conf->setValue("global/default_playlist", ui->tableView_defatul_playlist->result());
}
