#ifndef VIDEOCONTROLPANEL_H
#define VIDEOCONTROLPANEL_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>

class VideoWidget;
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
    explicit VideoControlPanel(VideoWidget *parent);


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
    QPushButton *createPanelButton(const QString &objectName, const QSize &size, QChar icon);
    void updateControlPanelState(ControlState state);

signals:
    void signalButtonPlay();
    void signalButtonPause();
    void signalButtonClose();
    void signalButtonFullScreen();
    void signalButtonFullScreenExit();
private:
    QTimer              *m_controlPanelTimer;//定时自动隐藏
    QPushButton         *m_buttonPlay;
    QPushButton         *m_buttonPause;
    QPushButton         *m_buttonClose;
    QPushButton         *m_buttonFullScreen;
    QPushButton         *m_buttonFullScreenExit;
    QLabel              *m_labelText;
    bool                m_isFullScreen;
    int                 m_processCur;
    int                 m_processTotal;
};

#endif // VIDEOCONTROLPANEL_H
