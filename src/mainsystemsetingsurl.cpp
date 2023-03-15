#include "mainsystemsetingsurl.h"
#include "ui_mainsystemsetingsurl.h"
#include "jsoncpppath.h"
#include "datasource.h"

#include <QtDebug>

MainSystemSetingsUrl::MainSystemSetingsUrl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainSystemSetingsUrl)
{
    ui->setupUi(this);
}

MainSystemSetingsUrl::~MainSystemSetingsUrl()
{
    delete ui;
}

void MainSystemSetingsUrl::refresh()
{
    ui->treeView->slotInitAll();
}

void MainSystemSetingsUrl::setDataSource(DataSource *datasource)
{
    m_datasource = datasource;
    ui->treeView->setDataSource(m_datasource);
}
