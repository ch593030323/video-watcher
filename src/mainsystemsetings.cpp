#include "mainsystemsetings.h"
#include "ui_mainsystemsetings.h"
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
#include <QMenu>
#include <QDir>
#include <QDirIterator>
#include <QButtonGroup>

MainSystemSetings::MainSystemSetings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainSystemSetings)
{
    ui->setupUi(this);

    //buttongroup
    ui->pushButton_play_alternate->setCheckable(true);
    ui->pushButton_play_form->setCheckable(true);
    QButtonGroup *buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(ui->pushButton_play_alternate);
    buttonGroup->addButton(ui->pushButton_play_form);
    buttonGroup->setExclusive(true);

    ui->pushButton_play_form->setChecked(true);
    toform();


    connect(ui->pushButton_play_alternate, SIGNAL(clicked()),this,SLOT(toalter()));
    connect(ui->pushButton_play_form, SIGNAL(clicked()),this,SLOT(toform()));
}

MainSystemSetings::~MainSystemSetings()
{
    delete ui;
}

void MainSystemSetings::toPlayFormNew()
{
//    playformnewdialog dialog(this);
    //dialog.setDataSource(ui->treeView->dataSource());
//    dialog.resize(this->size());
//    dialog.exec();
}

void MainSystemSetings::toPlayFormOK()
{
//    int row = ui->comboBox_form->currentIndex();
//    if(row < 0)
//        return;
//    QFile file(ui->comboBox_form->itemData(row).toString());
//    if(!file.open(QFile::ReadOnly)) {
//        qDebug() << file.errorString();
//    }
//    LayoutInfo info =  LayoutInfo::readFrom(file.readAll());
    //ui->widget_video->updateLayout(info);
}

void MainSystemSetings::toPlayAlternateNew()
{
    PlayAlternateNewDialog dialog(PlayAlternateNewDialog::TypeNew, this);
    dialog.resize(this->size());
    dialog.exec();
}

void MainSystemSetings::toPlayAlternateOK()
{
}

void MainSystemSetings::toalter()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainSystemSetings::toform()
{
    ui->stackedWidget->setCurrentIndex(1);
}
