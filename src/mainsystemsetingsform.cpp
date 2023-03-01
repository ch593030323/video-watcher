#include "mainsystemsetingsform.h"
#include "ui_mainsystemsetingsform.h"
#include "propertycolor.h"
#include "playformnewdialog.h"

#include <QFileInfo>
#include <QDateTime>
#include <QDir>
#include <QtDebug>

MainSystemSetingsForm::MainSystemSetingsForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainSystemSetingsForm)
{
    ui->setupUi(this);

    ui->pushButton_add->setIcon(PropertyColor::getFontPixmap(0x2b));
    ui->pushButton_del->setIcon(PropertyColor::getFontPixmap(0x2d));
    ui->pushButton_modify->setIcon(PropertyColor::getFontPixmap(0x2700));

    ui->tableView->appHorizontalHeader("name", QString::fromUtf8("名称"), 280);
    ui->tableView->appHorizontalHeader("size", QString::fromUtf8("大小"), 110);
    ui->tableView->appHorizontalHeader("create", QString::fromUtf8("创建日期"), 110);
    ui->tableView->appHorizontalHeader("enable", QString::fromUtf8("是否启用"), 110);
    ui->tableView->setEditTriggers(QTableView::NoEditTriggers);
    ui->tableView->setSelectionBehavior(QTableView::SelectRows);
    ui->tableView->setSelectionMode(QTableView::SingleSelection);

    connect(ui->pushButton_add, SIGNAL(clicked()),this,SLOT(toadd()));
    connect(ui->pushButton_modify, SIGNAL(clicked()),this,SLOT(tomodify()));
    connect(ui->pushButton_del, SIGNAL(clicked()),this,SLOT(todel()));

    refresh();
}

MainSystemSetingsForm::~MainSystemSetingsForm()
{
    delete ui;
}

void MainSystemSetingsForm::refresh()
{
    ui->tableView->removeRows(0, ui->tableView->rowCount());
    for(const QFileInfo &info : QDir("play_form").entryInfoList()) {
        if(!info.isFile())
            continue;
        ui->tableView->appendRow({info.fileName(),
                                  QString::number(info.size()) + "B",
                                  info.created().toString("yyyy-MM-dd")});
    }
    ui->tableView->selectRow(0);
}

void MainSystemSetingsForm::setDataSource(DataSource *datasource)
{
    m_datasource = datasource;
}

void MainSystemSetingsForm::toadd()
{
    playformnewdialog d(playformnewdialog::TypeNew, this);
    d.setDataSource(m_datasource);
    //TODO: exec作为插件有问题
    if(QDialog::Accepted == d.exec()) {
        refresh();
    }
}

void MainSystemSetingsForm::todel()
{
    int row = ui->tableView->currentIndex().row();
    if(row < 0)
        return;
    QFile::remove("play_form"  "/" + ui->tableView->data(row, "name").toString());
    refresh();
    ui->tableView->selectRow(row);
}

void MainSystemSetingsForm::tomodify()
{
    int row = ui->tableView->currentIndex().row();
    if(row < 0)
        return;

    playformnewdialog d(playformnewdialog::TypeModify, this);
    d.setDataSource(m_datasource);
    d.readFrom("play_form"  "/" + ui->tableView->data(row, "name").toString());
    //TODO: exec作为插件有问题
    d.exec();
}
