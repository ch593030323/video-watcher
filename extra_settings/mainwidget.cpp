#include "mainwidget.h"
#include "ui_mainwidget.h"
#include "json/json.h"
#include "src/treevideoviewsearchurl.h"
#include "src/treevideomodel.h"
#include "src/treevideoview.h"
#include "lds.h"

#include <QFile>
#include <QtDebug>
#include <QTreeView>
#include <QItemSelectionModel>
#include <QButtonGroup>
#include <QMessageBox>


mainWidget::mainWidget(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::mainWidget)
{
    ui->setupUi(this);
    this->setObjectName("Window");

    m_config = new JsonCppPath(lds::configDirectory + "/setting.json", this);
    if(!m_config->parse()) {
        qDebug() << m_config->errorString();
    }
    ui->spinBox_default_rowcount->setValue(m_config->value("/global/default_rowcount", 2).toInt());
    ui->spinBox_default_columncount->setValue(m_config->value("/global/default_columncount", 2).toInt());
    ui->tableView_default_playlist->setStringList(m_config->value("/global/default_play_list", QStringList()
                                                                  <<"http://vfx.mtime.cn/Video/2019/03/14/mp4/190314223540373995.mp4"
                                                                  <<"http://vfx.mtime.cn/Video/2021/01/07/mp4/210107172407759182_1080.mp4"
                                                                  <<"http://vfx.mtime.cn/Video/2019/03/19/mp4/190319212559089721.mp4"
                                                                  <<"http://vfx.mtime.cn/Video/2019/03/17/mp4/190317150237409904.mp")
                                                  .toStringList());
    m_datasource = new DataSource(this);

    //ui->treeView->hideMenu();
    ui->treeView->setDataSource(m_datasource);

    ui->pushButton_add->setIcon(lds::getFontPixmap(0x2b));
    ui->pushButton_sub->setIcon(lds::getFontPixmap(0x2d));

    //buttongroup
    ui->toolButton_basic->setCheckable(true);
    ui->toolButton_camera->setCheckable(true);
    QButtonGroup *buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(ui->toolButton_basic);
    buttonGroup->addButton(ui->toolButton_camera);
    buttonGroup->setExclusive(true);
    ui->toolButton_basic->setChecked(true);
    ui->stackedWidget->setCurrentIndex(0);

    connect(ui->pushButton_save, SIGNAL(clicked()), this, SLOT(tosave()));
    connect(ui->pushButton_exit, SIGNAL(clicked()), this, SLOT(toexit()));
    connect(buttonGroup, SIGNAL(buttonToggled(QAbstractButton *, bool)), this, SLOT(toSwitchPage(QAbstractButton *, bool)));
    connect(ui->pushButton_add, SIGNAL(clicked()), ui->tableView_default_playlist, SLOT(toAdd()));
    connect(ui->pushButton_sub, SIGNAL(clicked()), ui->tableView_default_playlist, SLOT(toRemove()));

    m_mapper = new EditedWidgetMapper(this);
    m_mapper->addObject(ui->spinBox_default_columncount, SIGNAL(editingFinished()));
    m_mapper->addObject(ui->spinBox_default_rowcount, SIGNAL(editingFinished()));
    m_mapper->addObject(ui->tableView_default_playlist, SIGNAL(editingFinished()));
    m_mapper->addObject(ui->treeView, SIGNAL(editingFinished()));


    toreload();
}

mainWidget::~mainWidget()
{
    delete ui;
}

void mainWidget::toexit()
{
    if(m_mapper->hasEditedWidget()) {
        if(0 == QMessageBox::question(this, "", tr("数据已修改，是否保存?"), tr("保存并退出"), tr("退出"))) {
            if(!saveJson()) {
                return;
            }
        }
    }
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
    saveJson();
}

bool mainWidget::saveJson()
{
    m_config->setValue("/global/default_rowcount", ui->spinBox_default_rowcount->value());
    m_config->setValue("/global/default_columncount", ui->spinBox_default_columncount->value());
    m_config->setValue("/global/default_play_list", ui->tableView_default_playlist->result());
    m_config->setValue("/device_list", m_datasource->cameraUrlCache);

    if(!m_config->saveToFile()) {
        QMessageBox::warning(this, "", m_config->errorString(), tr("确定"));
        return false;
    }
    QMessageBox::information(this, "", tr("保存成功"), tr("确定"));
    m_mapper->reset();
    return true;
}
