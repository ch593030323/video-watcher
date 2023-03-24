#include "mainsystemsetingssnap.h"
#include "ui_mainsystemsetingssnap.h"
#include "global.h"
#include "lds.h"
#include "autocloselabel.h"
#include "treevideoviewsearch.h"
#include "treevideoview.h"
#include "propertycolor.h"

#include <QDir>
#include <QStandardItemModel>
#include <QLabel>
#include <QtDebug>
#include <QSplashScreen>
#include <QWidgetAction>
#include <QMenu>
#include <QPainter>
#include <QImageReader>

MainSystemSetingsSnap::MainSystemSetingsSnap(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainSystemSetingsSnap)
{
    ui->setupUi(this);this->setWindowTitle(lds::windowName);
    m_noImageLabel = new QLabel(ui->listView);
    m_noImageLabel->hide();

    model = new QStandardItemModel(this);
    ui->listView->setModel(model);
    ui->listView->setViewMode(QListView::IconMode);
    ui->listView->setDragEnabled(false);
    ui->listView->setEditTriggers(QListView::NoEditTriggers);
    ui->listView->setResizeMode(QListView::Adjust);

    m_searchView = new TreeVideoViewSearch;
    m_menu = new QMenu(this);
    QWidgetAction *widgetaction=new QWidgetAction(ui->pushButton);
    widgetaction->setDefaultWidget(m_searchView);
    m_menu->addAction(widgetaction);
    ui->pushButton->setMenu(m_menu);

    ui->pushButton->setText(QString::fromUtf8("所有截图"));

    connect(ui->listView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slotShowMax(QModelIndex)));
    connect(ui->listView, SIGNAL(signalDelete()), this, SLOT(slotDeleteCurrentSnap()));
    connect(ui->listView, SIGNAL(signalRefresh()), this, SLOT(slotRefresh()));
    connect(m_menu, SIGNAL(aboutToShow()), this, SLOT(slotUpdateDevice()));
    connect(m_searchView->view(), SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slotSearchSnap(QModelIndex)));
}

MainSystemSetingsSnap::~MainSystemSetingsSnap()
{
    delete ui;
}

void MainSystemSetingsSnap::refresh()
{
    ui->pushButton->setText(QString::fromUtf8("所有截图"));
    ui->pushButton->setRoleData(VideoNameRole, "");
    ui->pushButton->setRoleData(VideoObidRole, "");
    slotRefresh();
}

void MainSystemSetingsSnap::refresh(const QString &obid)
{
    QString url = lds::dataSource->getCameraUrl(obid);
    model->removeRows(0, model->rowCount());
    for(auto k : QDir(lds::configDirectory + "/snap").entryInfoList()) {
        if(!k.isFile())
            continue;
        QImageReader reader(k.filePath());
        if(obid.isEmpty() || reader.text("url") == url) {
            QPixmap pixmap(k.filePath());
            QStandardItem *item = new QStandardItem;
            item->setData(pixmap.scaledToHeight(100), Qt::DecorationRole);
            item->setData(k.filePath(), FilePathRole);
            item->setToolTip(tr("名称：%1\n大小：%2 KB\n分辨率：%3x%4")
                             .arg(k.fileName())
                             .arg(k.size() / 1024)
                             .arg(pixmap.width()).arg(pixmap.height()));
            model->appendRow(item);
        }
    }

    if(model->rowCount() == 0) {
        QString text = "No Image";
        QPixmap pixmap(ui->listView->size());
        QPainter painter;
        painter.begin(&pixmap);
        painter.fillRect(pixmap.rect(), PropertyColor::viewColor);

        QFontMetrics fontm(painter.font());
        QPainterPath backgroundPath;
        backgroundPath.addRoundedRect(0, 0, fontm.width(text) * 2, fontm.height() * 2, 5, 5);
        QPoint pos = pixmap.rect().center() - backgroundPath.boundingRect().center().toPoint();
        painter.fillPath(backgroundPath.translated(pos),
                         PropertyColor::borderColor);

        painter.setPen(QPen(PropertyColor::textColor,  2));
        painter.drawText(pixmap.rect(), Qt::AlignCenter, "No Image");

        painter.end();

        m_noImageLabel->setAlignment(Qt::AlignCenter);
        m_noImageLabel->setGeometry(ui->listView->rect());
        m_noImageLabel->setPixmap(pixmap);
        m_noImageLabel->show();
    } else {
        m_noImageLabel->hide();
    }
}

void MainSystemSetingsSnap::refresh(const QModelIndex &index)
{
    slotSearchSnap(index);
}

void MainSystemSetingsSnap::slotRefresh()
{
    QString obid = ui->pushButton->roleData(VideoObidRole).toString();
    refresh(obid);
}

void MainSystemSetingsSnap::slotUpdateDevice()
{
    m_searchView->setDataSource(lds::dataSource);
    m_searchView->slotInitAll();
    QString obid = ui->pushButton->roleData(VideoObidRole).toString();
    if(obid.isEmpty())
        return;
    QModelIndex index = m_searchView->findItem(obid, VideoObidRole, TreeVideoViewSearch::Equal);
    if(!index.isValid())
        return;
    m_searchView->view()->setCurrentIndex(index);
}

void MainSystemSetingsSnap::slotSearchSnap(const QModelIndex &index)
{
    if(!index.isValid())
        return;

    if(index.data(VideoNodeTypeRole).toInt() != NodeTypeDevice)
        return;

    m_menu->close();
    ui->pushButton->setRoleData(VideoNameRole, index.data(VideoNameRole));
    ui->pushButton->setRoleData(VideoObidRole, index.data(VideoObidRole));
    ui->pushButton->setText(QString::fromUtf8("%1的截图")
                            .arg(index.data(VideoNameRole).toString()));
    refresh(index.data(VideoObidRole).toString());
}

void MainSystemSetingsSnap::slotShowMax(const QModelIndex &index)
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

void MainSystemSetingsSnap::slotDeleteCurrentSnap()
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
    slotRefresh();
}
