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

    VideoCell::parseVideoArea(m_layoutInfo,
                              this,
                              this->rect().adjusted(-lds::margin/2, -lds::margin/2, lds::margin/2, lds::margin/2),
                              m_videoMap);
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
                                         << ac_alter_stop);
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

QByteArray VideoWidget::layoutInfo2Json()
{
    return m_layoutInfo.toJson();
}

void VideoWidget::slotAddUrlToFocusedCell(const QString &url)
{
    VideoCell *cell = qobject_cast<VideoCell *>(qApp->focusWidget());
    if(!cell)
        return;

    cell->addPlayer(url);
}

void VideoWidget::slotAutoAddUrl(const QString &url)
{
    VideoCell *cell = qobject_cast<VideoCell *>(qApp->focusWidget());
    if(!cell) {
        for(QMap<LayoutPos, VideoCell *>::iterator k = m_videoMap.begin(); k != m_videoMap.end(); k ++) {
            qDebug() << __LINE__ << k.key().value() << k.value()->getInfo().url;
            if(k.value()->getInfo().url.isEmpty()) {
                cell = k.value();
                break;
            }
        }
    }
    if(!cell)
        return;
    cell->addPlayer(url);
}

void VideoWidget::resizeEvent(QResizeEvent *event)
{
    updateLayout();
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
