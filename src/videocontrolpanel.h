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
        Playing = 1,
        Paused = 2,
        FullScreen = 4,
        NormalScreen = 8,
        Close = 16,
        SaveImage = 32,

        Alter = Playing | Paused,
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
    void updateControlPanelState(ControlState state);

    void updateProgress(int cur, int total);

    void hideButtons(int flags);

    int buttonFlags();

protected:
    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    IconButton *createPanelButton(QChar icon, const QString &tipText);

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
