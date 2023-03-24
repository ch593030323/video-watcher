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

    /**
     * @brief updateLayout
     * 仅通过info来更新layout
     */
    void updateLayout(const LayoutInfo &info);
    /**
     * @brief updateLayout
     * 先VideoCell的info 更新到 layoutInfo里
     * 再通过info来更新layout
     */
    void updateLayout();

    void updateCheckState();

    /**
     * @brief toJson
     * 获取实时的VideoCell在内的layout json
     */
    QByteArray toJson();
    Json::Value toJsonValue();

    inline int columnCount() {return m_layoutInfo.column_count;}

public slots:
    void slotAutoAddUrl(const QModelIndex &index);

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
