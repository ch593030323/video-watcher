#include "videowidget.h"
#include "lds.h"

#include <QPainter>

VideoWidget::VideoWidget(QWidget *parent)
    : QWidget(parent)
    , m_isMultilselected(false)
{

}

void VideoWidget::updateLayout(const LayoutInfo &info)
{
    m_layoutInfo = info;

    VideoCell::updateVideoScene(m_layoutInfo,
                                this,
                                this->rect().adjusted(-lds::margin/2, -lds::margin/2, lds::margin/2, lds::margin/2),
                                m_videoMap);

    //信号和右键菜单
    if(m_isMultilselected) {
        //多选
        for(QMap<LayoutPos, VideoCell *>::iterator k = m_videoMap.begin(); k != m_videoMap.end(); k ++) {
            k.value()->setCheckable(true);
            disconnect(k.value(), SIGNAL(signalRelease()), this, SLOT(toselected()));
            connect(k.value(), SIGNAL(signalRelease()), this, SLOT(toselected()));
        }

        //清理选区
        tounselectedall();

        //添加右键菜单栏
        for(QMap<LayoutPos, VideoCell *>::iterator k = m_videoMap.begin(); k != m_videoMap.end(); k ++) {
            QList<VideoCell::ContextMenuData> pairList;
            pairList << VideoCell::ContextMenuData(QString::fromUtf8("合并所选"), this, SLOT(tomerge()));
            pairList << VideoCell::ContextMenuData(QString::fromUtf8("清除所有"), this, SLOT(toclear()));
            pairList << VideoCell::ContextMenuData(QString::fromUtf8("还原初始"), this, SLOT(torestore()));
            k.value()->setContextMenuDataList(pairList);
        }
    } else {
        //添加右键菜单栏
        for(QMap<LayoutPos, VideoCell *>::iterator k = m_videoMap.begin(); k != m_videoMap.end(); k ++) {
            VideoCell *cell = k.value();
            VideoCell::ContextMenuData ac_alter_start = VideoCell::ContextMenuData(QString::fromUtf8("开始轮播"),
                                                                                   this,
                                                                                   "",
                                                                                   "alter_start");
            VideoCell::ContextMenuData ac_alter_stop = VideoCell::ContextMenuData(QString::fromUtf8("停止轮播"),
                                                                                  cell,
                                                                                  SLOT(toAlterStop()),
                                                                                  "alter_stop");

            for(const QFileInfo &info : QDir("play_alter").entryInfoList()) {
                if(info.isFile())
                    ac_alter_start.children << VideoCell::ContextMenuData(
                                                   info.baseName(),
                                                   cell,
                                                   SLOT(toAlterStart()),
                                                   info.filePath());
            }
            cell->setContextMenuDataList(QList<VideoCell::ContextMenuData>()
                                         << ac_alter_start
                                         << ac_alter_stop
                                         << VideoCell::ContextMenuData(QString::fromUtf8("详细"),
                                                                       cell,
                                                                       SLOT(toShowDetail())));
        }
    }
}

void VideoWidget::updateLayout()
{
    updateLayoutInfo();
    updateLayout(m_layoutInfo);
}

void VideoWidget::updateLayoutInfo()
{
    m_layoutInfo.update(m_videoMap);
}

QByteArray VideoWidget::toJson()
{
    updateLayoutInfo();
    return m_layoutInfo.toJson();
}

void VideoWidget::slotAutoAddUrl(const QString &url)
{
    //search
    VideoCell *cell = qobject_cast<VideoCell *>(qApp->focusWidget());
    if(!cell) {
        for(QMap<LayoutPos, VideoCell *>::iterator k = m_videoMap.begin(); k != m_videoMap.end(); k ++) {
            if(k.value()->getInfo().url.isEmpty()) {
                cell = k.value();
                break;
            }
        }
    }
    if(!cell)
        return;
    //add
    cell->addPlayer(url);
}

void VideoWidget::resizeEvent(QResizeEvent *event)
{
    VideoCell::updateVideoGeometry(m_layoutInfo,
                                   this->rect().adjusted(-lds::margin/2, -lds::margin/2, lds::margin/2, lds::margin/2),
                                   m_videoMap);

}


QList<VideoCell *> VideoWidget::selectedWidgetList()
{
    QList<VideoCell *> r;
    for(QMap<LayoutPos, VideoCell *>::iterator k = m_videoMap.begin(); k != m_videoMap.end(); k ++) {
        if(k.value()->isChecked())
            r << k.value();
    }
    return r;
}

void VideoWidget::setMultilselected(bool isMultilselected)
{
    m_isMultilselected = isMultilselected;
}

void VideoWidget::tomerge()
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

    //clear cells in rect
    for(int x = 0; x < rect.width(); x ++) {
        for(int y = 0; y < rect.height(); y ++) {
            LayoutPos pos(x + rect.x(), y + rect.y());
            if(m_videoMap.contains(pos)) {
                m_videoMap[pos]->hide();
            }

            int index = LayoutCell::indexOf(pos, m_layoutInfo.cells);
            if(index >= 0) {
                m_layoutInfo.cells.removeAt(index);
            }
        }
    }
    //append rect
    m_layoutInfo.cells << cell;
    m_videoMap[cell.pos]->show();

    //清除选区
    tounselectedall();
    updateLayout();
}

void VideoWidget::toclear()
{
    tounselectedall();
}

void VideoWidget::torestore()
{
    m_layoutInfo = LayoutInfo(m_layoutInfo.column_count, m_layoutInfo.row_count);
    updateLayout();
}

void VideoWidget::toselected()
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
            //包含的选中
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

void VideoWidget::tounselectedall()
{
    for(QMap<LayoutPos, VideoCell *>::iterator k = m_videoMap.begin(); k != m_videoMap.end(); k ++) {
        k.value()->setChecked(false);
    }
}
