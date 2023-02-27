#ifndef VIDEOCONTROLPANEL_H
#define VIDEOCONTROLPANEL_H

#include "iconbutton.h"

#include <QWidget>
#include <QPushButton>
#include <QLabel>

class VideoCell;
class VideoControlPanel : public QWidget
{
    Q_OBJECT
public:
    enum ControlState{
        VideoPlay = 1,
        VideoPause = 2,
        VideoFullScreen = 4,
        VideoNormalScreen = 8,
        VideoClose = 16,
        VideoSaveImage = 32,

        VideoAlter = VideoPlay | VideoPause,
    };
    Q_DECLARE_FLAGS(ControlStates, ControlState)
public:
    explicit VideoControlPanel(VideoCell *parent);

    void showAutoHide(int timeout);

    inline bool isFullScreen() {
        return m_isFullScreen;
    }
    void setFullSreen(bool e);

    void setPause(bool e);

    void updateProgress(int cur, int total);

    void hideButtons(int flags);

    int buttonFlags();

protected:
    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    IconButton *createPanelButton(QChar icon, const QString &tipText);
    void updateControlPanelState(ControlState state);

signals:
    void signalButtonPlay();
    void signalButtonPause();
    void signalButtonClose();
    void signalButtonFullScreen();
    void signalButtonFullScreenExit();
    void signalButtonSaveImage();
private:
    QTimer              *m_controlPanelTimer;//定时自动隐藏
    IconButton          *m_buttonPlay;
    IconButton          *m_buttonPause;
    IconButton          *m_buttonClose;
    IconButton          *m_buttonFullScreen;
    IconButton          *m_buttonFullScreenExit;
    IconButton          *m_buttonSaveImage;
    QLabel              *m_labelText;
    bool                m_isFullScreen;
    int                 m_processCur;
    int                 m_processTotal;
};

#endif // VIDEOCONTROLPANEL_H
