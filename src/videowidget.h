#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QWidget>
#include "videoCell.h"
#include "videowidget.h"
#include "videolayout.h"

class VideoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit VideoWidget(QWidget *parent = nullptr);
    void updateLayout(const LayoutInfo &info);
    void updateLayout();

protected:
    virtual void resizeEvent(QResizeEvent *event);

private:
    QMap<LayoutPos, VideoCell *> m_videoMap;
    LayoutInfo m_layoutInfo;
};

#endif // VIDEOWIDGET_H
