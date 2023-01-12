#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include "json/json.h"
#include "lds.h"
#include "videolayout.h"

#include <QWidget>
#include <QFrame>
#include <QPushButton>
#include <QLabel>
#include <QVariantMap>

class VideoControlPanel;
class QMimeData;

class VideoWidget : public QWidget
{
    Q_OBJECT
public:

    struct ContextMenuData{
        ContextMenuData(){}
        ContextMenuData(QString text, QObject *target, const char *slot){
            this->text = text;
            this->target = target;
            this->slot = slot;
        }
        QString text;
        QObject *target;
        const char *slot;
    };

public:
    VideoWidget(LayoutPos pos, QWidget *parent = 0);
    ~VideoWidget();

    static void parseVideoArea(const LayoutInfo &info, QWidget *parentWidget, QRect area, QMap<LayoutPos, VideoWidget *> &cacheMap);

    void removePlayer();
    void addPlayer(const QString &deviceObid);

    void removeThread(QString device_obid);
    void addThread(QString device_obid);

    void updateControlPanelGeometry();

    void setCheckable(bool enabled);
    void setChecked(bool checked);
    bool isChecked();

    void setContextMenuDataList(QList<ContextMenuData> contextMenuDataList);

    /**
     * @brief setGeometryX 更新videowidget的geometry
     */
    void  setGeometryX(int column_spans, int row_spans, QRect area, int column_count, int row_count);
    QRect rectX();
    QString device_obid();
    const LayoutCell &getInfo();
    //=====================PlayList begin=====================
public:
    void addPlayerList(const QList<AlterPlayFrame> &playList);
    void runPlayerList();

    QList<AlterPlayFrame>   m_playList;//播放列表
    QString                 m_playListObid;//播放列表，当前播放的deviceobid
    QTimer                  *m_playListTimer;//播放列表的定时器
    int                     m_playListIndex;//播放列表的序号
private slots:
    void updatePlayListDevice();
    //=====================PlayList end=====================
private slots:
    void updateImage(const FFmpegData &d);

    void toControlPlay();
    void toControlPause();
    void toControlClose();
    void toControlFullScreen();
    void toControlFullScreenExit();

signals:
    void signalRelease();

protected:
    void paintEvent(QPaintEvent *);
    void hideEvent(QHideEvent *event);

    void dragEnterEvent(QDragEnterEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);

    //drag this view
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void contextMenuEvent(QContextMenuEvent * event);

    //focus
    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);

    void resizeEvent(QResizeEvent *event);

    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);

private:
    FFmpegData                  m_ffmpegData;//播放数据
    VideoControlPanel           *m_controlPanel;//顶部面板控制

    QPoint                      m_pressPos;//鼠标按下的位置
    bool                        m_isPressed;//鼠标是否按下
    bool                        m_pressMoving;//鼠标是否按下并移动

    bool                        m_isChecked;//是否被选中
    bool                        m_isCheckable;//是否可以被选中

    LayoutCell                  m_info;//记录layoutcell的数据
    QList<ContextMenuData>      m_contextMenuDataList;//右键菜单的action信息

public:
    static VideoWidget          *lastFocusWidget; //记录最后一次的焦点控件指针
};


#endif // VIDEOWIDGET_H
