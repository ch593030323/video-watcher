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


    m_settingsAlter = qobject_cast<MainSystemSetingsAlter *>(ui->stackedWidget->widget(0));
    m_settingsForm = qobject_cast<MainSystemSetingsForm *>(ui->stackedWidget->widget(1));


    connect(ui->pushButton_play_alternate, SIGNAL(clicked()),this,SLOT(toalter()));
    connect(ui->pushButton_play_form, SIGNAL(clicked()),this,SLOT(toform()));
}

MainSystemSetings::~MainSystemSetings()
{
    delete ui;
}

void MainSystemSetings::setDataSource(DataSource *datasource)
{
    m_datasource = datasource;
    m_settingsForm->setDataSource(m_datasource);
    m_settingsAlter->setDataSource(m_datasource);
}

void MainSystemSetings::toalter()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainSystemSetings::toform()
{
    ui->stackedWidget->setCurrentIndex(1);
}
