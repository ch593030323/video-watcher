#include "videowidget.h"
#include "lds.h"

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
    VideoCell::parseVideoArea(m_layoutInfo, this, this->rect(), m_videoMap);
}

void VideoWidget::resizeEvent(QResizeEvent *event)
{
    updateLayout();
}
