#include "videowidget.h"
#include "lds.h"

#include <QPainter>

VideoWidget::VideoWidget(QWidget *parent) : QWidget(parent)
{

}

void VideoWidget::updateLayout(const LayoutInfo &info)
{
    m_layoutInfo = info;
    updateLayout();
}

void VideoWidget::updateLayout()
{
    VideoCell::parseVideoArea(m_layoutInfo,
                              this,
                              this->rect().adjusted(-lds::margin/2, -lds::margin/2, lds::margin/2, lds::margin/2),
                              m_videoMap);
}

void VideoWidget::resizeEvent(QResizeEvent *event)
{
    updateLayout();
}
