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

void MainSystemSetings::setFlat(QPushButton *button, const QPixmap &pixmap)
{
    button->setIcon(pixmap);
    button->setFlat(true);
}

MainSystemSetings::MainSystemSetings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainSystemSetings)
{
    ui->setupUi(this);

    //buttongroup
    ui->pushButton_play_alternate->setCheckable(true);
    ui->pushButton_play_form->setCheckable(true);
    ui->pushButton_snap->setCheckable(true);
    ui->pushButton_url->setCheckable(true);
    QButtonGroup *buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(ui->pushButton_play_alternate);
    buttonGroup->addButton(ui->pushButton_play_form);
    buttonGroup->addButton(ui->pushButton_snap);
    buttonGroup->addButton(ui->pushButton_url);
    buttonGroup->setExclusive(true);

    ui->pushButton_play_form->setChecked(true);

    m_settingsAlter = qobject_cast<MainSystemSetingsAlter *>(ui->stackedWidget->widget(0));
    m_settingsForm = qobject_cast<MainSystemSetingsForm *>(ui->stackedWidget->widget(1));
    m_settingsSnap = qobject_cast<MainSystemSetingsSnap *>(ui->stackedWidget->widget(2));
    m_settingsUrl = qobject_cast<MainSystemSetingsUrl *>(ui->stackedWidget->widget(3));

    connect(ui->pushButton_play_alternate, SIGNAL(clicked()),this,SLOT(toalter()));
    connect(ui->pushButton_play_form, SIGNAL(clicked()),this,SLOT(toform()));
    connect(ui->pushButton_snap, SIGNAL(clicked()),this,SLOT(tosnap()));
    connect(ui->pushButton_url, SIGNAL(clicked()),this,SLOT(tourl()));
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
    m_settingsUrl->setDataSource(m_datasource);
}

void MainSystemSetings::refresh()
{
    ui->pushButton_play_form->setChecked(true);
    toform();
}

void MainSystemSetings::toalter()
{
    ui->stackedWidget->setCurrentIndex(0);
    m_settingsAlter->refresh();
}

void MainSystemSetings::toform()
{
    ui->stackedWidget->setCurrentIndex(1);
    m_settingsForm->refresh();
}

void MainSystemSetings::tosnap()
{
    ui->stackedWidget->setCurrentIndex(2);
    m_settingsSnap->refresh();
}

void MainSystemSetings::tourl()
{
    ui->stackedWidget->setCurrentIndex(3);
    m_settingsUrl->refresh();
}
