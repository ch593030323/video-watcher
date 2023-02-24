#include "videowidget.h"
#include "lds.h"
#include "ffmpeg.h"
#include "mainwindow.h"
#include "playformnewdialog.h"
#include "videocontrolpanel.h"
#include "propertycolor.h"

#include <QPainter>
#include <QtDebug>
#include <QDragEnterEvent>
#include <QApplication>
#include <QDrag>
#include <QMimeData>
#include <QMenu>
#include <QLineF>
#include "mainvideowidget.h"

VideoCell::VideoCell(LayoutPos pos, QWidget *parent)
    : QWidget(parent)
    , m_isPressed(false)
    , m_pressMoving(false)
    , m_isCheckable(false)
    , m_isChecked(false)
    , m_playListIndex(0)
{
    m_info.pos = pos;

    this->setFocusPolicy(Qt::StrongFocus);
    this->setAcceptDrops(true);
    this->setMouseTracking(true);

    //m_controlPanel
    m_controlPanel = new VideoControlPanel(this);
    m_controlPanel->hide();
    //
    m_playListTimer = new QTimer(this);
    connect(m_playListTimer, SIGNAL(timeout()), this, SLOT(updatePlayListDevice()));

    connect(m_controlPanel,  SIGNAL(signalButtonPlay()), this, SLOT(toControlPlay()));
    connect(m_controlPanel,  SIGNAL(signalButtonPause()), this, SLOT(toControlPause()));
    connect(m_controlPanel,  SIGNAL(signalButtonClose()), this, SLOT(toControlClose()));
    connect(m_controlPanel,  SIGNAL(signalButtonFullScreen()), this, SLOT(toControlFullScreen()));
    connect(m_controlPanel,  SIGNAL(signalButtonFullScreenExit()), this, SLOT(toControlFullScreenExit()));
    connect(m_controlPanel,  SIGNAL(signalButtonSaveImage()), this, SLOT(toControlSaveImage()));
}

VideoCell::~VideoCell()
{
    removeThread(m_info.url);
}

void VideoCell::parseVideoArea(const LayoutInfo &info, QWidget *parentWidget, QRect area, QMap<LayoutPos, VideoCell *> &cacheMap)
{
    const int column_count =  info.column_count;
    const int row_count = info.row_count;

    //用于后面隐藏播放控件
    QList<VideoCell *>unused_video_widget_list = cacheMap.values();

    //更新所有的cell的geometry
    for(int row = 0; row < row_count; row ++) {
        for(int column = 0; column < column_count; column ++) {
            LayoutPos pos = LayoutPos(column, row);
            if(!cacheMap.contains(pos)) {
                cacheMap.insert(pos, new VideoCell(pos, parentWidget));
            }
            VideoCell *w = cacheMap.value(pos);
            w->setGeometryX(1, 1, area, column_count, row_count);

            unused_video_widget_list.removeOne(w);
        }
    }

    //只更新info里cell的geometry
    for(int k = 0; k < info.cells.count(); k ++) {
        const LayoutCell &cell = info.cells[k];
        //
        LayoutPos pos = cell.pos;
        if(!cacheMap.contains(pos)) {
            cacheMap.insert(pos, new VideoCell(pos, parentWidget));
        }
        VideoCell *w = cacheMap.value(pos);
        w->addPlayer(cell.url);

        unused_video_widget_list.removeOne(w);

        w->setGeometryX(cell.column_spans, cell.row_spans, area, column_count, row_count);
        //将不需要显示的cell保存在unused_video
        for(int p1 = 0; p1 < cell.column_spans; p1 ++) {
            for(int p2 = 0; p2 < cell.row_spans; p2 ++) {
                if(p1 == 0 && p2 == 0)
                    continue;
                LayoutPos pos = LayoutPos(cell.x() + p1, cell.y() + p2);
                VideoCell *w = cacheMap.value(pos, NULL);
                if(w) {
                    unused_video_widget_list.append(w);
                }
            }
        }
    }

    //hide unused video widget
    QList<VideoCell *>all_video_widget_list = cacheMap.values();
    for(int k = 0; k < all_video_widget_list.count(); k ++) {
        VideoCell *w = all_video_widget_list[k];

        if(unused_video_widget_list.contains(w)) {
            w->removePlayer();
            w->hide();
        } else {
            w->show();
        }
    }
}

void VideoCell::updateImage(const FFmpegData &d)
{
    //播放、暂停的控制命令
    if(d.type == FFmpegData::Control) {
        if(FFmpegData::Playing == d.playState) {
            m_controlPanel->setPause(false);
        }
        if(FFmpegData::Paused == d.playState) {
            m_controlPanel->setPause(true);
        }
        if(FFmpegData::Stopped == d.playState) {
            //m_controlPanel->setPause(true);
        }
        return;
    }
    //轮播控制
    if(m_playListTimer->isActive()) {
        FFmpegThread *thread = qobject_cast<FFmpegThread *>(sender());
        if(!thread)
            return;
        if(PlayThread::PlayThreadMap.value(m_playListUrl, NULL) != thread)
            return;
    }
    m_ffmpegData = d;
    m_controlPanel->updateProgress(d.cur, d.total);
    update();
}

void VideoCell::toControlPlay()
{
    PlayThread *thread = PlayThread::PlayThreadMap.value(m_info.url, NULL);
    if(!thread)
        return;
    thread->play();

//    m_controlPanel->setPause(false);
}

void VideoCell::toControlPause()
{
    PlayThread *thread = PlayThread::PlayThreadMap.value(m_info.url, NULL);
    if(!thread)
        return;
    thread->pause();
    //
//    m_controlPanel->setPause(true);
}

void VideoCell::toControlClose()
{
    removePlayer();
}

void VideoCell::toControlFullScreen()
{
    if(this->parentWidget()) {
        this->setGeometry(0, 0, this->window()->width(), this->window()->height());
        this->setParent(this->window());
        this->show();
        this->raise();
    }
    m_controlPanel->setFullSreen(true);
}

void VideoCell::toControlFullScreenExit()
{
    VideoWidget *pw = this->parent()->findChild<VideoWidget *>();
    if(!pw)
        return;
    this->setParent(pw);
    this->setFocus();

    m_controlPanel->setFullSreen(false);
    pw->updateLayout();
}

void VideoCell::toControlSaveImage()
{
    QString path = "snap";
    QDir().mkpath(path);
    m_ffmpegData.image.save(path + "/" + lds::getUniqueFileNamehByDateTime(path) + ".png");
}

void VideoCell::updatePlayListDevice()
{
    Q_ASSERT(m_playList.count());

    m_playListIndex++;
    m_playListIndex = m_playListIndex % m_playList.count();

    if(m_playListIndex < 0 || m_playListIndex >= m_playList.count())
        return;
    m_playListUrl = m_playList[m_playListIndex].url;
    m_playListTimer->setInterval(m_playList[m_playListIndex].timeout * 1000);
}

void VideoCell::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QRect paint_rect = this->rect().adjusted(lds::margin / 2, lds::margin / 2, -lds::margin / 2, -lds::margin / 2);
    painter.fillRect(paint_rect, PropertyColor::viewColor);
    if(FFmpegData::NoError == m_ffmpegData.errorCode) {
        //后台播放，直接removePlayer会有残留图片，故增加obidIsValid参数
        const QImage &image = m_ffmpegData.image;
        bool obidIsValid = (m_info.url != "" || m_playListUrl != "");
        if(!image.isNull() && obidIsValid) {
            int r1 = image.width() * 10 / image.height();
            int r2 = paint_rect.width() * 10 / paint_rect.height();
            QSize target_size;
            if(r1 > r2) {
                target_size = QSize(paint_rect.width(), paint_rect.width() / (r1 / 10.0));
            } else {
                target_size = QSize(paint_rect.height() * (r1 / 10.0), paint_rect.height());
            }
            QRect target_rect = QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, target_size, paint_rect);
            painter.drawImage(target_rect, image, image.rect());
        }
    } else {
        //绘制异常报错
        painter.save();
        painter.setPen(PropertyColor::highlightColor);
        painter.drawText(this->rect(), Qt::AlignCenter | Qt::TextWordWrap, m_ffmpegData.errorString);
        painter.restore();
    }

    //绘制边框
    bool paintBorder = false;
    if(m_isCheckable) {
        paintBorder = m_isChecked;
    } else if(hasFocus()) {
        paintBorder = true;
    }
    if(paintBorder) {
        painter.setPen(QPen(PropertyColor::highlightColor, 2));
        painter.drawRect(paint_rect.adjusted(1, 1, -1, -1));
    }
}

void VideoCell::hideEvent(QHideEvent *event)
{
//    removePlayer();
    QWidget::hideEvent(event);
}

void VideoCell::dragEnterEvent(QDragEnterEvent *event)
{
    const QMimeData *mimedata = event->mimeData();
    mimedata->formats();//这里起刷新formats的作用
    if (mimedata->hasFormat("camera/url")) {
        event->accept();
        return;
    }
    if (mimedata->hasFormat("VideoCell/url") && mimedata->data("VideoCell/pos").toInt() != m_info.pos.value()) {
        event->accept();
        return;
    }
    //end
    event->ignore();
}

void VideoCell::dragLeaveEvent(QDragLeaveEvent *event)
{

}


void VideoCell::dragMoveEvent(QDragMoveEvent *event)
{
    const QMimeData *mimedata = event->mimeData();
    mimedata->formats();//这里起刷新formats的作用
    if (mimedata->hasFormat("camera/url")) {
        event->accept();
        return;
    }
    if (mimedata->hasFormat("VideoCell/url") && mimedata->data("VideoCell/pos").toInt() != m_info.pos.value()) {
        event->accept();
        return;
    }
    //end
    event->ignore();
}

void VideoCell::dropEvent(QDropEvent *event)
{
    //event->mimeData()->formats()起到刷新的作用，此处不删除
    QStringList formats = event->mimeData()->formats();
    if (event->mimeData()->hasFormat("camera/url")) {
        QString url = event->mimeData()->data("camera/url");
        addPlayer(url);
        event->setDropAction(Qt::CopyAction);
        event->accept();
        return;
    }

    if (event->mimeData()->hasFormat("VideoCell/url")) {
        QByteArray url = event->mimeData()->data("VideoCell/url");
        // add new
        this->addPlayer(url);
        this->setFocus();
        event->setDropAction(Qt::CopyAction);
        event->accept();

        //remove old
        VideoCell *videocell = qobject_cast<VideoCell *>(event->source());
        videocell->removePlayer();
        return;
    }

    //end
    event->ignore();

}

void VideoCell::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton) {
        event->ignore();
        return;
    }
    m_pressPos = event->pos();
    m_isPressed = true;
}

void VideoCell::mouseMoveEvent(QMouseEvent *event)
{
    /*
     * 前提:VideoControlPanel上不移动几秒后不自动隐藏，VideoWidget上则自动隐藏
     * 但是VideoControlPanel上move时，会先触发自己的moveEvent然后再触发VideoWidget的moveEvent，导致自动隐藏功一直生效
     * 这里加个geometry().contains的判断来规避上述情况
    */
    if(m_info.url.isEmpty()) {
        //obid无效
    } else if(m_controlPanel->isVisible() && !m_controlPanel->geometry().contains(event->pos())) {
        //面板显示且光标不在面板上
        m_controlPanel->showAutoHide(1000);
    } else if(!m_controlPanel->isVisible()) {
        //面板不显示
        m_controlPanel->showAutoHide(1000);
    }

    //当移动的偏移量 大于 startDragDistance时才会触发拖拽
    if((event->pos() - m_pressPos).manhattanLength() < qApp->startDragDistance()
            || m_pressMoving
            || !m_isPressed
            || m_info.url.isEmpty()) {

    } else {
        m_pressMoving = true;

        //此处必须是new QDrag
        QDrag *drag = new QDrag(this);
        QMimeData *m = new QMimeData;
        m->setData("VideoCell/url", m_info.url.toLocal8Bit());
        m->setData("VideoCell/pos", QByteArray::number(m_info.pos.value()));

        QPixmap pix = QPixmap::grabWidget(this, this->rect()).scaled(50, 50);
        drag->setPixmap(pix);
        drag->setHotSpot(QPoint(10, 10));
        drag->setMimeData(m);
        drag->exec();//Qt::CopyAction, Qt::IgnoreAction);

        m_pressMoving = false;
        m_isPressed = false;
    }

    QWidget::mouseMoveEvent(event);
}

void VideoCell::mouseReleaseEvent(QMouseEvent *event)
{
    m_isPressed = false;
    QWidget::mouseReleaseEvent(event);

    if(m_isCheckable) {
        setChecked(true);//!m_isChecked;
    }
    update();
    emit signalRelease();
}

void VideoCell::contextMenuEvent(QContextMenuEvent *event)
{
    if(m_contextMenuDataList.isEmpty())
        return;
    QMenu m(this);
    for(int k = 0; k < m_contextMenuDataList.count(); k ++) {
        connect(m.addAction(m_contextMenuDataList[k].text),
                SIGNAL(triggered()),
                m_contextMenuDataList[k].target,
                m_contextMenuDataList[k].slot);
    }
    m.move(this->mapToGlobal(event->pos()));
    m.exec();
}

void VideoCell::removePlayer()
{
    //移除轮播
    if(m_playListTimer->isActive()) {
        for(int k = 0; k < m_playList.count(); k ++) {
            removeThread(m_playList[k].url);
        }
        m_playListTimer->stop();
        m_playList.clear();
        m_playListUrl = "";
        m_playListIndex = 0;
    }
    //close cur thread
    removeThread(m_info.url);

    //还原菜单栏
    m_info.url = "";
    if(m_controlPanel->isFullScreen())
        toControlFullScreenExit();

    //clear play screen
    m_ffmpegData.init();
    m_controlPanel->hide();
    update();
}

void VideoCell::removeThread(QString url)
{
    PlayThread *thread = PlayThread::PlayThreadMap.value(url, NULL);
    if(!thread)
        return;
    disconnect(thread, SIGNAL(receiveImage(FFmpegData)), this, SLOT(updateImage(FFmpegData)));
    qDebug() << __LINE__ << "ffmpeg thread connection count is " << thread->receiverImageConnectionCount();
    if(thread->receiverImageConnectionCount() == 0) {
        thread->close();
    }
}

void VideoCell::addThread(QString url)
{
    PlayThread *thread = PlayThread::PlayThreadMap.value(url, NULL);
    if(!thread)
        return;
    thread->open();
    connect(thread, SIGNAL(receiveImage(FFmpegData)), this, SLOT(updateImage(FFmpegData)), Qt::QueuedConnection);
    //qDebug() << "ffmpeg thread connection count is " << thread->getUrl() << thread->receiverImageConnectionCount();

}

void VideoCell::addPlayer(const QString &url)
{
    if(m_info.url == url)
        return;

    PlayThread *thread = PlayThread::PlayThreadMap.value(url, NULL);
    if(thread == NULL) {
        thread = PlayThread::createPlayThread(url);
        PlayThread::PlayThreadMap.insert(url, thread);
    }

    removePlayer();

    addThread(url);

    //播放器的定义为：一个播放线程一个播放器，VideoCell是一个播放窗口
    //无状态 正在播放
    if(thread->lastPlayState() == FFmpegData::NoState
            || thread->lastPlayState() == FFmpegData::Playing) {
        m_controlPanel->setPause(false);
    }
    //播放暂停
    if(thread->lastPlayState() == FFmpegData::Paused) {
        m_controlPanel->setPause(true);
        updateImage(thread->lastPlayData());
    }
    //播放结束
    if(thread->lastPlayState() == FFmpegData::Stopped) {
        m_controlPanel->setPause(false);
        thread->play();
    }
    qDebug() << __LINE__ << thread->lastPlayState();

    m_info.url = url;
}

void VideoCell::addPlayerList(const QList<AlterPlayFrame> &playList)
{
    removePlayer();

    m_playList = playList;
    for(int k = 0; k < playList.count(); k ++) {
        addThread(playList[k].url);
    }
    //
    toControlPlay();

    runPlayerList();
}

void VideoCell::runPlayerList()
{
    m_playListIndex = -1;
    m_playListTimer->start();
    updatePlayListDevice();
}

void VideoCell::updateControlPanelGeometry()
{    
    m_controlPanel->setGeometry(lds::margin / 2, lds::margin / 2, this->width() - lds::margin, 40);
}

void VideoCell::setCheckable(bool enabled)
{
    m_isCheckable = enabled;
}

void VideoCell::setChecked(bool checked)
{
    m_isChecked = checked;
    update();
}

bool VideoCell::isChecked()
{
    return m_isChecked;
}

void VideoCell::setContextMenuDataList(QList<ContextMenuData> contextMenuDataList)
{
    m_contextMenuDataList = contextMenuDataList;
}

void VideoCell::setGeometryX(int column_spans, int row_spans, QRect area, int column_count, int row_count)
{
    m_info.column_spans = column_spans;
    m_info.row_spans = row_spans;

    if(m_controlPanel->isFullScreen()) {
        toControlFullScreen();
    } else {
        QRect rect = m_info.rect();

        Q_ASSERT(column_count > 0 && row_count > 0);
        int cell_width = area.width() / column_count;
        int cell_height = area.height() / row_count;
        this->setGeometry(QRect(rect.x()     * cell_width,
                                rect.y()     * cell_height,
                                rect.width() * cell_width,
                                rect.height()* cell_height).translated(area.topLeft()));
    }
}

QRect VideoCell::rectX()
{
    return m_info.rect();
}

const LayoutCell &VideoCell::getInfo()
{
    return m_info;
}

void VideoCell::focusInEvent(QFocusEvent *event)
{
    QWidget::focusInEvent(event);
}

void VideoCell::focusOutEvent(QFocusEvent *event)
{
    QWidget::focusOutEvent(event);
}

void VideoCell::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updateControlPanelGeometry();
}

void VideoCell::enterEvent(QEvent *event)
{
    QWidget::enterEvent(event);
}

void VideoCell::leaveEvent(QEvent *event)
{
    m_controlPanel->hide();
    QWidget::leaveEvent(event);
}
