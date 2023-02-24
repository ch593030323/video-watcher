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
        VideoPlay,
        VideoPause,
        VideoFullScreen,
        VideoNormalScreen
    };
public:
    explicit VideoControlPanel(VideoCell *parent);


    void showAutoHide(int timeout);

    inline bool isFullScreen() {
        return m_isFullScreen;
    }
    inline void setFullSreen(bool e) {
        m_isFullScreen = e;
        updateControlPanelState(e ? VideoFullScreen : VideoNormalScreen);
    }

    inline void setPause(bool e) {
        updateControlPanelState(e ? VideoPause : VideoPlay);
    }

    void updateProgress(int cur, int total);

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
