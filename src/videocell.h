#ifndef VIDEOCELL_H
#define VIDEOCELL_H

#include "json/json.h"
#include "lds.h"
#include "videolayout.h"
#include "playthread.h"
#include "videocontrolpanel.h"

#include <QWidget>
#include <QFrame>
#include <QPushButton>
#include <QLabel>
#include <QVariantMap>

class VideoControlPanel;
class QMimeData;
class VideoWidget;

class VideoCell : public QWidget
{
    Q_OBJECT
public:

    struct ContextMenuData {
        ContextMenuData(){}
        ContextMenuData(QString text, QObject *target, const char *slot, const QVariant &var = QVariant()){
            this->text = text;
            this->target = target;
            this->slot = slot;
            this->var = var;
        }
        QString text;
        QObject *target;
        const char *slot;
        QVariant var;

        QList<ContextMenuData> children;
    };

public:
    VideoCell(LayoutPos pos, QWidget *parent = 0);
    ~VideoCell();

    static VideoCell *createCell(LayoutPos pos, VideoWidget *parentWidget);
    static void updateVideoScene(const LayoutInfo &info, VideoWidget *parentWidget, QRect area, QMap<LayoutPos, VideoCell *> &cacheMap);
    static void updateVideoGeometry(const LayoutInfo &info, QRect area, QMap<LayoutPos, VideoCell *> &cacheMap);

    void removePlayer();
    void addPlayer(const QString &url, const QString &obid);
    void preparePlayer(const QString &url);

    void removeThread(const QString &url);
    PlayThread *addThread(const QString &url);

    void updateControlPanelGeometry();

    void setChecked(bool checked);
    bool isChecked();
    void setCheckable(bool isCheckable);

    /**
     * @brief setGeometryX 更新videowidget的geometry
     */
    void  setGeometryX(int column_spans, int row_spans, QRect area, int column_count, int row_count);
    QRect rectX();
    const LayoutCell &getInfo();
    //=====================PlayList begin=====================
public:
    void addPlayerList(const QList<AlterPlayFrame> &playList);
    void runPlayerList();

    QList<AlterPlayFrame>   m_playList;//播放列表
    QString                 m_playListUrl;//播放列表，当前播放的deviceobid
    QTimer                  *m_playListTimer;//播放列表的定时器
    int                     m_playListIndex;//播放列表的序号
private slots:
    void updatePlayListDevice();
public slots:
    void toAlterStart();
    void toAlterStop();
    void toShowDetail();
    void toAlterNew();

    //=====================PlayList end=====================
private slots:
    void updateImage(const FFmpegData &d);

    void toControlPlay();
    void toControlPause();
    void toControlClose();
    void toControlFullScreen();
    void toControlFullScreenExit();
    void toControlSaveImage();

signals:
    void signalRelease();
    void signalAlterMerge();
    void signalAlterClear();
    void signalAlterRestore();

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
    void addActionsToMenu(const QList<ContextMenuData> &list, QMenu *sub_menu, QMenu *parent_menu);

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
};


#endif // VIDEOCELL_H
