#include "videowidget.h"
#include "lds.h"
#include "ffmpeg.h"
#include "mainwindow.h"
#include "playformnewdialog.h"
#include "videocontrolpanel.h"
#include "propertycolor.h"
#include "mainvideowidget.h"
#include "playalternatenewdialog.h"
#include "videowidget.h"

#include <QPainter>
#include <QtDebug>
#include <QDragEnterEvent>
#include <QApplication>
#include <QDrag>
#include <QMimeData>
#include <QMenu>
#include <QLineF>

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

VideoCell *VideoCell::createCell(LayoutPos pos, VideoWidget *parentWidget)
{
    VideoCell *w = new VideoCell(pos, parentWidget);
    connect(w, SIGNAL(signalRelease()),         parentWidget, SLOT(toselected()));
    connect(w, SIGNAL(signalAlterClear()),      parentWidget, SLOT(toclear()));
    connect(w, SIGNAL(signalAlterMerge()),      parentWidget, SLOT(tomerge()));
    connect(w, SIGNAL(signalAlterRestore()),    parentWidget, SLOT(torestore()));
    return w;
}

void VideoCell::updateVideoScene(const LayoutInfo &info, VideoWidget *parentWidget, QRect area, QMap<LayoutPos, VideoCell *> &cacheMap)
{
    const int column_count =  info.column_count;
    const int row_count = info.row_count;

    //用于后面隐藏播放控件
    QList<VideoCell *>unused_video_widget_list = cacheMap.values();

    //更新所有的cell的geometry
    for(int row = 0; row < row_count; row ++) {
        for(int column = 0; column < column_count; column ++) {
            LayoutPos pos = LayoutPos(column, row);

            VideoCell *w = cacheMap.value(pos, 0);
            if(!w) {
                w = createCell(pos, parentWidget);
                cacheMap.insert(pos, w);
            }

            unused_video_widget_list.removeOne(w);
        }
    }

    //只更新info里cell的geometry
    for(int k = 0; k < info.cells.count(); k ++) {
        const LayoutCell &cell = info.cells[k];
        LayoutPos pos = cell.pos;

        VideoCell *w = cacheMap.value(pos, 0);
        if(!w) {
            w = createCell(pos, parentWidget);
            cacheMap.insert(pos, w);
        }
        w->addPlayer(cell.url);

        unused_video_widget_list.removeOne(w);

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

    //隐藏无用的cell
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

    //移除不在layoutinfo里的播放器
    for(int row = 0; info.cells.count() > 0 && row < row_count; row ++) {
        for(int column = 0; column < column_count; column ++) {
            LayoutPos pos = LayoutPos(column, row);
            int index = LayoutCell::indexOf(pos, info.cells);
            if(index < 0) {
                VideoCell *w = cacheMap.value(pos, 0);
                if(w)
                    w->removePlayer();
            }

        }
    }


    updateVideoGeometry(info, area, cacheMap);
}

void VideoCell::updateVideoGeometry(const LayoutInfo &info, QRect area, QMap<LayoutPos, VideoCell *> &cacheMap)
{
    const int column_count =  info.column_count;
    const int row_count = info.row_count;
    //更新所有的cell的geometry
    for(int row = 0; row < row_count; row ++) {
        for(int column = 0; column < column_count; column ++) {
            LayoutPos pos = LayoutPos(column, row);
            VideoCell *w = cacheMap.value(pos, 0);
            if(w)
                w->setGeometryX(1, 1, area, column_count, row_count);
        }
    }


    //只更新info里cell的geometry
    for(int k = 0; k < info.cells.count(); k ++) {
        const LayoutCell &cell = info.cells[k];
        //
        LayoutPos pos = cell.pos;
        VideoCell *w = cacheMap.value(pos, 0);
        if(w)
            w->setGeometryX(cell.column_spans, cell.row_spans, area, column_count, row_count);
    }
}

void VideoCell::updateImage(const FFmpegData &d)
{
    //播放、暂停的控制命令
    if(d.type == FFmpegData::Control) {
        if(FFmpegData::Playing == d.playState) {
            m_controlPanel->updateControlPanelState(VideoControlPanel::Playing);
        }
        if(FFmpegData::Paused == d.playState) {
            m_controlPanel->updateControlPanelState(VideoControlPanel::Paused);
        }
        if(FFmpegData::Stopped == d.playState) {
            //m_controlPanel->updateControlPanelState(VideoControlPanel::Paused);
        }
    }
    if(d.errorCode != FFmpegData::NoError) {
        m_controlPanel->updateControlPanelState(VideoControlPanel::Paused);
    }
    if(d.type == FFmpegData::Control) {
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

    //    m_controlPanel->updateControlPanelState(VideoControlPanel::Playing);
}

void VideoCell::toControlPause()
{
    PlayThread *thread = PlayThread::PlayThreadMap.value(m_info.url, NULL);
    if(!thread)
        return;
    thread->pause();
    //
    //    m_controlPanel->updateControlPanelState(VideoControlPanel::Paused);
}

void VideoCell::toControlClose()
{
    removePlayer();
}

void VideoCell::toControlFullScreen()
{
    //look for window
    QWidget *pw = this->parentWidget();
    while(pw && !pw->isWindow())
        pw = pw->parentWidget();

    if(pw) {
        this->setGeometry(0, 0, pw->width(), pw->height());
        this->setParent(pw);
        this->show();
        this->raise();
        m_controlPanel->setFullSreen(true);
    }
}

void VideoCell::toControlFullScreenExit()
{
    VideoWidget *pw = this->parent()->findChild<VideoWidget *>();
    if(!pw)
        return;
    this->setParent(pw);
    this->setFocus();
    this->show();//widget will hide after changed parent

    m_controlPanel->setFullSreen(false);
    pw->updateLayout();
}

void VideoCell::toControlSaveImage()
{
    QString url = m_info.url;
    if(m_playListTimer->isActive())
        url = m_playListUrl;

    PlayThread *thread = PlayThread::PlayThreadMap.value(url);
    if(!thread)
        return;

    thread->snap();

}

void VideoCell::updatePlayListDevice()
{
    Q_ASSERT(m_playList.count());

    m_playListIndex++;
    m_playListIndex = m_playListIndex % m_playList.count();
    if(m_playListIndex < 0 || m_playListIndex >= m_playList.count())
        return;

    lds::showMessage((QString("正在轮播, 坐标:(%1,%2), 进度:%3/%4, 名称:%5, 时长:%6s")
                      .arg(m_info.pos.x + 1)
                      .arg(m_info.pos.y + 1)
                      .arg(m_playListIndex + 1)
                      .arg(m_playList.count())
                      .arg(m_playList[m_playListIndex].url.section("/", -1))
                      .arg(m_playList[m_playListIndex].length)
                      ));
    m_playListUrl = m_playList[m_playListIndex].url;
    m_playListTimer->setInterval(m_playList[m_playListIndex].length * 1000);
}

void VideoCell::toAlterStop()
{
    removePlayer();
    lds::showMessage("轮播结束");
}

void VideoCell::toShowDetail()
{
    PlayThread *thread = PlayThread::PlayThreadMap.value(m_info.url);
    if(!thread)
        return;
    qDebug() << "url:"   << m_info.url
             << "state:" << thread->lastPlayState()
             << "data:"  << thread->lastPlayData().errorString;
}

void VideoCell::toAlterNew()
{
    PlayAlternateNewDialog d(PlayAlternateNewDialog::TypeNew, this);
    d.setDataSource(lds::dataSource);
    if(QDialog::Accepted == d.exec()) {

    }
}

void VideoCell::toAlterStart()
{
    QAction *ac = qobject_cast<QAction *>(this->sender());
    if(!ac)
        return;
    QString filepath = ac->data().toString();

    addPlayerList(AlterPlayFrame::readFrom(filepath));
    lds::showMessage("轮播开始");
    runPlayerList();
}

void VideoCell::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QRect paint_rect = this->rect().adjusted(lds::margin / 2, lds::margin / 2, -lds::margin / 2, -lds::margin / 2);
    painter.fillRect(paint_rect, PropertyColor::viewColor);
    if(FFmpegData::NoError == m_ffmpegData.errorCode) {
        //后台播放，直接removePlayer会有残留图片，故增加isValid参数
        const QImage &image = m_ffmpegData.image;
        bool isValid = (m_info.url != "" || m_playListUrl != "");
        if(!image.isNull() && isValid) {
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
    if(m_controlPanel->isFullScreen()) {
        paintBorder = false;
    } else if(m_isCheckable) {
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
    if (mimedata->hasFormat("VideoCell/url")
            && mimedata->data("VideoCell/pos").toInt() != m_info.pos.value()) {
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
    if (mimedata->hasFormat("VideoCell/url")
            && mimedata->data("VideoCell/pos").toInt() != m_info.pos.value()) {
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
    if(m_info.url.isEmpty() && m_playList.isEmpty()) {
        //obid无效
    } else if(m_controlPanel->isVisible() && !m_controlPanel->geometry().contains(event->pos())) {
        //面板显示且光标不在面板上
        m_controlPanel->showAutoHide(1000);
    } else if(!m_controlPanel->isVisible()) {
        //面板不显示
        m_controlPanel->showAutoHide(1000);
    }

    if(!m_playList.isEmpty()) {
        m_controlPanel->hideButtons(m_controlPanel->buttonFlags() | VideoControlPanel::Alter);
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
        emit signalRelease();
    }
    update();
}

void VideoCell::contextMenuEvent(QContextMenuEvent *event)
{
    //actionList
    QList<VideoCell::ContextMenuData> actionList;
    //添加组播键菜单
    if(m_isCheckable) {
        //添加右键菜单栏
        actionList << VideoCell::ContextMenuData(QString::fromUtf8("合并所选"), this, SIGNAL(signalAlterMerge()));
        actionList << VideoCell::ContextMenuData(QString::fromUtf8("清除所有"), this, SIGNAL(signalAlterClear()));
        actionList << VideoCell::ContextMenuData(QString::fromUtf8("还原初始"), this, SIGNAL(signalAlterRestore()));
    } else {

        //添加轮播菜单

        VideoCell::ContextMenuData ac_alter_start = VideoCell::ContextMenuData(QString::fromUtf8("开始轮播"),
                                                                               this,
                                                                               "",
                                                                               "alter_start");
        VideoCell::ContextMenuData ac_alter_stop = VideoCell::ContextMenuData(QString::fromUtf8("停止轮播"),
                                                                              this,
                                                                              SLOT(toAlterStop()),
                                                                              "alter_stop");

        for(const QFileInfo &info : QDir(lds::configDirectory + "/play_alter").entryInfoList()) {
            if(info.isFile())
                ac_alter_start.children << VideoCell::ContextMenuData(
                                               info.baseName(),
                                               this,
                                               SLOT(toAlterStart()),
                                               info.filePath());
        }

        actionList << ac_alter_start;
        actionList << ac_alter_stop;
        if(ac_alter_start.children.isEmpty()) {
            actionList << VideoCell::ContextMenuData(QString::fromUtf8("新增轮播"),
                                                     this,
                                                     SLOT(toAlterNew()));
        }
    }
    //menu
    QMenu m(this);
    addActionsToMenu(actionList, &m, 0);
    m.move(this->mapToGlobal(event->pos()));
    m.exec();

    //右键菜单后，还原为单选状态
    VideoWidget *pw = qobject_cast<VideoWidget *>(this->parent());
    if(pw) {
        pw->setMultilselected(false);
        pw->updateCheckState();
    }
}

void VideoCell::addActionsToMenu(const QList<VideoCell::ContextMenuData> &list, QMenu *sub_menu, QMenu *parent_menu)
{
    if(!sub_menu)
        return;

    for(int k = 0; k < list.count(); k ++) {
        const ContextMenuData &d = list[k];

        //add action
        QAction *ac = 0;
        QMenu *menu = 0;
        if(d.children.count() > 0) {
            menu = new QMenu(sub_menu);
            ac = menu->menuAction();
        } else {
            ac = new QAction(0);
        }

        ac->setText(d.text);
        ac->setData(d.var);
        sub_menu->addAction(ac);

        if(!QByteArray(d.slot).isEmpty()) {
            connect(ac, SIGNAL(triggered()), d.target, d.slot);
        }

        //action enabled
        if(m_playListTimer->isActive() && d.var.toString() == "alter_start") {
            ac->setEnabled(false);
        }
        if(!m_playListTimer->isActive() && d.var.toString() == "alter_stop") {
            ac->setEnabled(false);
        }
        if(d.var.toString() == "alter_start" && d.children.isEmpty()) {
            ac->setEnabled(false);
        }

        //menu children
        addActionsToMenu(d.children, menu, sub_menu);

        //menu root
        if(0 != parent_menu) {
            QAction *mac = sub_menu->menuAction();
            parent_menu->addAction(mac);
        }
    }
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

void VideoCell::removeThread(const QString &url)
{
    PlayThread *thread = PlayThread::PlayThreadMap.value(url, NULL);
    if(!thread)
        return;
    disconnect(thread, SIGNAL(receiveImage(FFmpegData)), this, SLOT(updateImage(FFmpegData)));
    qDebug() << __FUNCTION__ << "ffmpeg thread connection count is " << thread->receiverImageConnectionCount();
    if(thread->receiverImageConnectionCount() == 0) {
        thread->close();
    }
}

PlayThread *VideoCell::addThread(const QString &url)
{
    PlayThread *thread = PlayThread::PlayThreadMap.value(url, NULL);
    if(thread == NULL) {
        thread = PlayThread::createPlayThread(url);
        PlayThread::PlayThreadMap.insert(url, thread);
    }
    thread->open();
    connect(thread, SIGNAL(receiveImage(FFmpegData)), this, SLOT(updateImage(FFmpegData)), Qt::QueuedConnection);
    qDebug() << __FUNCTION__ << "ffmpeg thread connection count is " << thread->receiverImageConnectionCount();

    return thread;
}

void VideoCell::addPlayer(const QString &url)
{
    if(m_info.url == url)
        return;

    removePlayer();
    m_info.url = url;

    preparePlayer(url);
}

void VideoCell::preparePlayer(const QString &url)
{
    PlayThread *thread = addThread(url);

    //播放器的定义为：一个播放线程一个播放器，VideoCell是一个播放窗口
    //无状态 正在播放
    if(thread->lastPlayState() == FFmpegData::NoState
            || thread->lastPlayState() == FFmpegData::Playing) {
        //m_controlPanel->updateControlPanelState(VideoControlPanel::Playing);
    }
    //播放暂停
    if(thread->lastPlayState() == FFmpegData::Paused) {
        m_controlPanel->updateControlPanelState(VideoControlPanel::Paused);
        updateImage(thread->lastPlayData());
    }
    //播放结束
    if(thread->lastPlayState() == FFmpegData::Stopped) {
        //m_controlPanel->updateControlPanelState(VideoControlPanel::Playing);
        thread->play();
    }
}

void VideoCell::addPlayerList(const QList<AlterPlayFrame> &playList)
{
    removePlayer();

    m_playList = playList;
    for(int k = 0; k < playList.count(); k ++) {
        preparePlayer(playList[k].url);
    }
    //
    //    toControlPlay();

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

void VideoCell::setChecked(bool checked)
{
    m_isChecked = checked;
    update();
}

bool VideoCell::isChecked()
{
    return m_isChecked;
}

void VideoCell::setCheckable(bool isCheckable)
{
    m_isCheckable = isCheckable;
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


void VideoCell::keyPressEvent(QKeyEvent *e)
{
    if(Qt::Key_Shift == e->key()) {
        VideoWidget *pw = qobject_cast<VideoWidget *>(this->parent());
        if(pw) {
            pw->setMultilselected(true);
            pw->updateCheckState();
        }
    }
    QWidget::keyPressEvent(e);
}

void VideoCell::keyReleaseEvent(QKeyEvent *e)
{
    if(Qt::Key_Shift == e->key()) {
        VideoWidget *pw = qobject_cast<VideoWidget *>(this->parent());
        if(pw) {
            pw->setMultilselected(false);
            pw->updateCheckState();
        }
    }
    QWidget::keyPressEvent(e);
}
