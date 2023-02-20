#include "playformnewdialog.h"
#include "ui_playformnewdialog.h"
#include "videocell.h"
#include "lds.h"

#include <QStandardItemModel>
#include <QApplication>
#include <QDesktopWidget>

static QList<LayoutInfo> LayoutInfos =  QList<LayoutInfo>()
        << LayoutInfo(1, 1)
        << LayoutInfo(2, 2)
        << LayoutInfo(3, 3)
        << LayoutInfo(4, 4)
        << LayoutInfo(5, 5)
           ;

playformnewdialog::playformnewdialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::playformnewdialog)
{
    ui->setupUi(this);
    setObjectName("Window");
    setDataSource(new DataSource(this));
    ui->pushButton_1x1->setIcon(lds::getLayoutPixmap(1));
    ui->pushButton_1x1->setText("");
    ui->pushButton_2x2->setIcon(lds::getLayoutPixmap(2));
    ui->pushButton_2x2->setText("");
    ui->pushButton_3x3->setIcon(lds::getLayoutPixmap(3));
    ui->pushButton_3x3->setText("");
    ui->pushButton_4x4->setIcon(lds::getLayoutPixmap(4));
    ui->pushButton_4x4->setText("");
    ui->pushButton_5x5->setIcon(lds::getLayoutPixmap(5));
    ui->pushButton_5x5->setText("");

    //connect
    connect(ui->pushButton_1x1, SIGNAL(clicked()), this, SLOT(toVideoLayout1x1()));
    connect(ui->pushButton_2x2, SIGNAL(clicked()), this, SLOT(toVideoLayout2x2()));
    connect(ui->pushButton_3x3, SIGNAL(clicked()), this, SLOT(toVideoLayout3x3()));
    connect(ui->pushButton_4x4, SIGNAL(clicked()), this, SLOT(toVideoLayout4x4()));
    connect(ui->pushButton_5x5, SIGNAL(clicked()), this, SLOT(toVideoLayout5x5()));

    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(toaccept()));
    connect(ui->pushButton_exit, SIGNAL(clicked()), this, SLOT(toexit()));
    connect(ui->pushButton_merge, SIGNAL(clicked()), this, SLOT(tomerge()));
    connect(ui->pushButton_clear, SIGNAL(clicked()), this, SLOT(toclear()));
    toVideoLayout3x3();

    ui->pushButton_clear->hide();
    ui->pushButton_merge->hide();
}

playformnewdialog::~playformnewdialog()
{
    delete ui;
}


void playformnewdialog::setDataSource(DataSource *datasource)
{
    ui->treeView->setDataSource(datasource);
    ui->treeView->slotInitTree();
}

void playformnewdialog::resizeEvent(QResizeEvent *event)
{
    updateLayout();
}

void playformnewdialog::updateLayout(bool isRemovePlayer)
{
    ui->widget_video->updateLayout(m_layoutInfo);
//    int videoAreaWidth = this->width() - lds::videoAreaRight - lds::margin;
//    int videoAreaHeight = this->height() - lds::videoAreaBottom - lds::margin;
//    int videoAreaX = lds::margin / 2;
//    int videoAreaY = lds::margin / 2 + lds::border_width;
//    int treeHeight = videoAreaHeight - lds::margin / 2 - lds::border_width;

//    //treeView
//    ui->treeView->setGeometry(this->width() - lds::videoAreaRight, lds::margin, lds::videoAreaRight - lds::margin, treeHeight);
//    //videoType
//    int buttonTop = this->height() - lds::videoAreaBottom;// + (lds::videoAreaBottom - b40) / 2;
//    ui->pushButton_1x1->setGeometry(lds::margin + 0,   buttonTop, 50, b40);
//    ui->pushButton_2x2->setGeometry(lds::margin + 70,  buttonTop, 50, b40);
//    ui->pushButton_3x3->setGeometry(lds::margin + 140, buttonTop, 50, b40);
//    ui->pushButton_4x4->setGeometry(lds::margin + 210, buttonTop, 50, b40);
//    ui->pushButton_5x5->setGeometry(lds::margin + 280, buttonTop, 50, b40);

//    ui->pushButton_merge->setGeometry(this->width() - 400,  buttonTop, 80, b40);
//    ui->pushButton_clear->setGeometry(this->width() - 300,  buttonTop, 80, b40);
//    ui->pushButton_ok->setGeometry(this->width() - 200,     buttonTop, 80, b40);
//    ui->pushButton_exit->setGeometry(this->width() - 100,   buttonTop, 80, b40);
//    //videoWidget
//    VideoCell::parseVideoArea(m_layoutInfo, this, QRect(videoAreaX, videoAreaY, videoAreaWidth, videoAreaHeight), m_videoMap);

//    //videoWidget设置成选区模式
//    for(QMap<LayoutPos, VideoCell *>::iterator k = m_videoMap.begin(); k != m_videoMap.end(); k ++) {
//        k.value()->setCheckable(true);
//        disconnect(k.value(), SIGNAL(signalRelease()), this, SLOT(toselected()));
//        connect(k.value(), SIGNAL(signalRelease()), this, SLOT(toselected()));
//    }
//    //清理选区
//    tounselectedall();
//    //是否会移除播放器
//    if(isRemovePlayer) {
//        for(QMap<LayoutPos, VideoCell *>::iterator k = m_videoMap.begin(); k != m_videoMap.end(); k ++) {
//            k.value()->removePlayer();
//        }
//    }
//    //添加右键菜单栏
//    for(QMap<LayoutPos, VideoCell *>::iterator k = m_videoMap.begin(); k != m_videoMap.end(); k ++) {
//        QList<VideoCell::ContextMenuData> pairList;
//        pairList << VideoCell::ContextMenuData(QString::fromUtf8("合并"), this, SLOT(tomerge()));
//        pairList << VideoCell::ContextMenuData(QString::fromUtf8("清除"), this, SLOT(toclear()));
//        pairList << VideoCell::ContextMenuData(QString::fromUtf8("还原"), this, SLOT(torestore()));
//        k.value()->setContextMenuDataList(pairList);

//    }
}

QList<VideoCell *> playformnewdialog::selectedWidgetList()
{
    QList<VideoCell *> r;
    for(QMap<LayoutPos, VideoCell *>::iterator k = m_videoMap.begin(); k != m_videoMap.end(); k ++) {
        if(k.value()->isChecked())
            r << k.value();
    }
    return r;
}

void playformnewdialog::contextMenuEvent(QContextMenuEvent *event)
{
    QWidget::contextMenuEvent(event);
}

void playformnewdialog::toVideoLayout1x1()
{
    m_layoutInfo = LayoutInfos[0];
    updateLayout(true);
}

void playformnewdialog::toVideoLayout2x2()
{
    m_layoutInfo = LayoutInfos[1];
    updateLayout(true);
}

void playformnewdialog::toVideoLayout3x3()
{
    m_layoutInfo = LayoutInfos[2];
    updateLayout(true);
}

void playformnewdialog::toVideoLayout4x4()
{
    m_layoutInfo = LayoutInfos[3];
    updateLayout(true);
}

void playformnewdialog::toVideoLayout5x5()
{
    m_layoutInfo = LayoutInfos[4];
    updateLayout(true);
}

void playformnewdialog::toaccept()
{
    m_layoutInfo.update(m_videoMap);

    QDir().mkpath("play_form");
    QFile file("play_form/form.txt");
    if(!file.open(QFile::WriteOnly)) {
        qDebug() << file.errorString();
    }
    file.write(m_layoutInfo.toJson());
    file.close();

    //    this->accept();
}

void playformnewdialog::toexit()
{
    this->reject();
}

void playformnewdialog::tomerge()
{
    QList<VideoCell *> selectedList = selectedWidgetList();
    if(selectedList.count() == 0)
        return;
    //合并单元格
    QRect rect;
    for(int k = 0; k < selectedList.count(); k ++) {
        rect = rect.united(selectedList[k]->rectX());
    }

    LayoutCell cell;
    cell.pos.x = rect.x();
    cell.pos.y = rect.y();
    cell.column_spans = rect.width();
    cell.row_spans = rect.height();
    m_layoutInfo.cells << cell;

    //layoutinfo移除无用的cell
    QList<int> willRemoveList;
    const QList<LayoutCell> &cells = m_layoutInfo.cells;
    for(int k1 = 0; k1 < cells.count(); k1 ++) {
        for(int k2 = k1 + 1; k2 < cells.count(); k2 ++) {
            if(willRemoveList.contains(k1) || willRemoveList.contains(k2))
                continue;

            if(cells[k1].rect().contains(cells[k2].rect())) {
                willRemoveList << k2;
            } else if(cells[k2].rect().contains(cells[k1].rect())) {
                willRemoveList << k1;
            }
        }
    }
    qSort(willRemoveList.begin(), willRemoveList.end(), qGreater<int>());
    for(int k = 0; k < willRemoveList.count(); k ++) {
        m_layoutInfo.cells.removeAt(willRemoveList[k]);
    }

    //清除选区
    tounselectedall();
    updateLayout();
}

void playformnewdialog::toclear()
{
    tounselectedall();
}

void playformnewdialog::torestore()
{
    m_layoutInfo = LayoutInfo(m_layoutInfo.column_count, m_layoutInfo.row_count);
    updateLayout(true);
}

void playformnewdialog::toselected()
{
    VideoCell *w = qobject_cast<VideoCell *>(sender());
    if(!w)
        return;


    QList<VideoCell *> selectedList = selectedWidgetList();
    QRect rect;
    for(int k = 0; k < selectedList.count(); k ++) {
        rect = rect.united(selectedList[k]->geometry());
    }

    for(QMap<LayoutPos, VideoCell *>::iterator k = m_videoMap.begin(); k != m_videoMap.end(); ) {

        if(!k.value()->isVisible()) {
            //没有显示的跳过处理
        } else if(k.value()->isChecked()) {
            //已选中的不做处理
        } else if(rect.contains(k.value()->geometry())) {
            k.value()->setChecked(true);
        } else if(rect.intersected(k.value()->geometry()).isValid()) {
            //遇到不包含但有交集的rect，需要重新计算rect
            rect = rect.united(k.value()->geometry());
            k = m_videoMap.begin();
            continue;
        }

        k ++;
    }
}

void playformnewdialog::tounselectedall()
{
    for(QMap<LayoutPos, VideoCell *>::iterator k = m_videoMap.begin(); k != m_videoMap.end(); k ++) {
        k.value()->setChecked(false);
    }
}
