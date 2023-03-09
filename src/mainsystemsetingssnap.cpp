#include "mainsystemsetingssnap.h"
#include "ui_mainsystemsetingssnap.h"
#include "global.h"
#include "lds.h"
#include "autocloselabel.h"

#include <QDir>
#include <QStandardItemModel>
#include <QLabel>
#include <QtDebug>
#include <QSplashScreen>

MainSystemSetingsSnap::MainSystemSetingsSnap(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainSystemSetingsSnap)
{
    ui->setupUi(this);
    model = new QStandardItemModel(this);
    ui->listView->setModel(model);
    ui->listView->setViewMode(QListView::IconMode);
    ui->listView->setDragEnabled(false);
    ui->listView->setEditTriggers(QListView::NoEditTriggers);
    ui->listView->setResizeMode(QListView::Adjust);

    connect(ui->listView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(toShowMax(QModelIndex)));
    connect(ui->listView, SIGNAL(signalDelete()), this, SLOT(toDeleteCurrentSnap()));
    connect(ui->listView, SIGNAL(signalRefresh()), this, SLOT(toRefresh()));

//    toRefresh();
}

MainSystemSetingsSnap::~MainSystemSetingsSnap()
{
    delete ui;
}

void MainSystemSetingsSnap::refresh()
{
    toRefresh();
}

void MainSystemSetingsSnap::toRefresh()
{
    model->removeRows(0, model->rowCount());
    for(auto k : QDir(lds::configDirectory + "/snap").entryInfoList()) {
        if(k.isFile()) {
            QStandardItem *item = new QStandardItem;
            item->setData(QPixmap(k.filePath()).scaledToHeight(100), Qt::DecorationRole);
            item->setData(k.filePath(), FilePathRole);
            item->setText(k.fileName());
            model->appendRow(item);
        }
    }
}

void MainSystemSetingsSnap::toShowMax(const QModelIndex &index)
{
    QPixmap pixmap(index.data(FilePathRole).toString());
    AutoCloseLabel *w = new AutoCloseLabel(ui->listView);
    w->setAttribute(Qt::WA_DeleteOnClose);
    w->setStyleSheet("background:black;");
    w->setAlignment(Qt::AlignCenter);
    w->setGeometry(ui->listView->rect());
    w->setPixmap(pixmap.scaled(ui->listView->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    w->show();
}

void MainSystemSetingsSnap::toDeleteCurrentSnap()
{
    QModelIndex index = ui->listView->currentIndex();
    if(!index.isValid())
        return;

    QFile file(index.data(FilePathRole).toString());
    if(!file.remove()) {
        lds::showMessage(tr("删除文件") , file.errorString());
        return;
    }
    lds::showMessage(tr("删除成功"));
    toRefresh();
}
