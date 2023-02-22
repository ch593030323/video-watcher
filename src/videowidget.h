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

    void updateLayoutInfo();
    QByteArray layoutInfo2Json();

    inline LayoutInfo getLayoutInfo() {return m_layoutInfo;}

public slots:
    void slotAddUrlToFocusedCell(const QString &url);

protected:
    virtual void resizeEvent(QResizeEvent *event);

private:
    QMap<LayoutPos, VideoCell *> m_videoMap;
    LayoutInfo m_layoutInfo;

public:
    /*----------------------------多选begin----------------------------*/
    QList<VideoCell *> selectedWidgetList();
    void setMultilselected(bool isMultilselected);
public slots:
    void tomerge();
    void toclear();
    void torestore();

    void toselected();
    void tounselectedall();
private:
    bool m_isMultilselected;
    /*----------------------------多选end----------------------------*/
};

#endif // VIDEOWIDGET_H
