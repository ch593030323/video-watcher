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
    if(m_isMultilselected) {
        for(QMap<LayoutPos, VideoCell *>::iterator k = m_videoMap.begin(); k != m_videoMap.end(); k ++) {
            k.value()->setCheckable(true);
        }
    }
}

void VideoWidget::updateLayout()
{
    m_layoutInfo.update(m_videoMap);
    updateLayout(m_layoutInfo);
}

QByteArray VideoWidget::toJson()
{
    m_layoutInfo.update(m_videoMap);
    return m_layoutInfo.toJson();
}

Json::Value VideoWidget::toJsonValue()
{
    m_layoutInfo.update(m_videoMap);
    return m_layoutInfo.toJsonValue();
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
    updateLayout(LayoutInfo(m_layoutInfo.column_count, m_layoutInfo.row_count));
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
