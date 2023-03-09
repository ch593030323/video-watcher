#include "mainsystemsetingsalter.h"
#include "ui_mainsystemsetingsalter.h"
#include "propertycolor.h"
#include "playalternatenewdialog.h"
#include "global.h"

#include <QFileInfo>
#include <QDateTime>
#include <QDir>
#include <QtDebug>

MainSystemSetingsAlter::MainSystemSetingsAlter(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainSystemSetingsAlter)
{
    ui->setupUi(this);

    ui->pushButton_add->setIcon(PropertyColor::getFontPixmap(0x2b));
    ui->pushButton_del->setIcon(PropertyColor::getFontPixmap(0xf014));
    ui->pushButton_modify->setIcon(PropertyColor::getFontPixmap(0x2700));

    ui->tableView->appHorizontalHeader("name", QString::fromUtf8("名称"), 280);
    ui->tableView->appHorizontalHeader("size", QString::fromUtf8("大小"), 110);
    ui->tableView->appHorizontalHeader("create", QString::fromUtf8("创建日期"), 120);
    ui->tableView->appHorizontalHeader("enable", QString::fromUtf8("是否启用"), 110);
    ui->tableView->setEditTriggers(QTableView::NoEditTriggers);
    ui->tableView->setSelectionBehavior(QTableView::SelectRows);
    ui->tableView->setSelectionMode(QTableView::SingleSelection);

    connect(ui->pushButton_add, SIGNAL(clicked()),this,SLOT(toadd()));
    connect(ui->pushButton_modify, SIGNAL(clicked()),this,SLOT(tomodify()));
    connect(ui->pushButton_del, SIGNAL(clicked()),this,SLOT(todel()));

//    refresh();
}

MainSystemSetingsAlter::~MainSystemSetingsAlter()
{
    delete ui;
}

void MainSystemSetingsAlter::refresh()
{
    ui->tableView->removeRows(0, ui->tableView->rowCount());
    for(const QFileInfo &info : QDir(lds::configDirectory + "/play_alter").entryInfoList()) {
        if(!info.isFile())
            continue;
        ui->tableView->appendRow({info.fileName(),
                                  QString::number(info.size()) + "B",
                                  info.created().toString("yyyy-MM-dd")});
        ui->tableView->setData(ui->tableView->rowCount() - 1, "name", info.filePath(), FilePathRole);
    }
    ui->tableView->selectRow(0);
}

void MainSystemSetingsAlter::setDataSource(DataSource *datasource)
{
    m_datasource = datasource;
}

void MainSystemSetingsAlter::toadd()
{
    PlayAlternateNewDialog d(PlayAlternateNewDialog::TypeNew, this);
    d.setDataSource(m_datasource);
    if(QDialog::Accepted == d.exec()) {
        refresh();
    }
}

void MainSystemSetingsAlter::todel()
{
    int row = ui->tableView->currentIndex().row();
    if(row < 0)
        return;
    QFile::remove(ui->tableView->data(row, "name", FilePathRole).toString());
    refresh();
    ui->tableView->selectRow(row);
}

void MainSystemSetingsAlter::tomodify()
{
    int row = ui->tableView->currentIndex().row();
    if(row < 0)
        return;

    PlayAlternateNewDialog d(PlayAlternateNewDialog::TypeModify, this);
    d.setDataSource(m_datasource);
    d.readFrom(ui->tableView->data(row, "name", FilePathRole).toString());
    if(QDialog::Accepted == d.exec()) {
        refresh();
    }
}
