#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QWidget>
#include "videocell.h"
#include "videowidget.h"
#include "videolayout.h"

class VideoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit VideoWidget(QWidget *parent = 0);

    void updateLayout(const LayoutInfo &info);
    void updateLayout();

    void updateLayoutInfo();
    QByteArray toJson();

    inline int columnCount() {return m_layoutInfo.column_count;}

public slots:
    void slotAutoAddUrl(const QString &url);

protected:
    virtual void resizeEvent(QResizeEvent *event);

private:
    QMap<LayoutPos, VideoCell *> m_videoMap;
    LayoutInfo m_layoutInfo;

    /*----------------------------多选begin----------------------------*/
public:
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
