#include "videocontrolpanel.h"
#include "lds.h"
#include "videocell.h"
#include "propertycolor.h"
#include "iconbutton.h"

VideoControlPanel::VideoControlPanel(VideoCell *parent)
    : QWidget(parent)
    , m_isFullScreen(false)
    , m_processCur(0)
    , m_processTotal(0)
{
    setObjectName("controlPanel");
    this->setMouseTracking(true);

    m_controlPanelTimer = new QTimer(this);
    m_controlPanelTimer->setSingleShot(true);

    m_labelText = new QLabel;
    m_labelText->setProperty("outer_stylesheet", "sliderTip");

    m_buttonPlay            = createPanelButton(0x25b6, QString::fromUtf8("播放"));
    m_buttonPause           = createPanelButton(0x23f8, QString::fromUtf8("暂停"));
    m_buttonClose           = createPanelButton(0x23f9, QString::fromUtf8("关闭"));
    m_buttonFullScreen      = createPanelButton(0xf424, QString::fromUtf8("全屏"));
    m_buttonFullScreenExit  = createPanelButton(0xf422, QString::fromUtf8("退出全屏"));
    m_buttonSaveImage       = createPanelButton(0xf03e, QString::fromUtf8("截图"));
    m_buttonFullScreenExit->hide();

    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->addWidget(m_labelText, 1);
    hlayout->addWidget(m_buttonSaveImage);
    hlayout->addWidget(m_buttonPlay);
    hlayout->addWidget(m_buttonPause);
    hlayout->addWidget(m_buttonClose);
    hlayout->addWidget(m_buttonFullScreen);
    hlayout->addWidget(m_buttonFullScreenExit);
    hlayout->setSpacing(5);
    hlayout->setMargin(5);
    this->setLayout(hlayout);

    connect(m_controlPanelTimer, SIGNAL(timeout()), this, SLOT(hide()));

    connect(m_buttonPlay,           SIGNAL(clicked()), this, SIGNAL(signalButtonPlay()));
    connect(m_buttonPause,          SIGNAL(clicked()), this, SIGNAL(signalButtonPause()));
    connect(m_buttonClose,          SIGNAL(clicked()), this, SIGNAL(signalButtonClose()));
    connect(m_buttonFullScreen,     SIGNAL(clicked()), this, SIGNAL(signalButtonFullScreen()));
    connect(m_buttonFullScreenExit, SIGNAL(clicked()), this, SIGNAL(signalButtonFullScreenExit()));
    connect(m_buttonSaveImage,      SIGNAL(clicked()), this, SIGNAL(signalButtonSaveImage()));
}


IconButton *VideoControlPanel::createPanelButton(QChar icon, const QString &tipText)
{
    IconButton *b = new IconButton;
    b->setIconChar(icon);
    b->setFocusPolicy(Qt::NoFocus);
    b->setToolTip(tipText);
    b->setFixedSize(QSize(15, 15));
    b->setType(IconButton::ToolButton);
    return b;
}

void VideoControlPanel::showAutoHide(int timeout)
{
    this->show();
    m_controlPanelTimer->start(timeout);
}

void VideoControlPanel::updateProgress(int cur, int total)
{
    //当直播时，total小于零，即无播放时长
    if(total < 0)
        return;
    if(m_processCur == cur && m_processTotal == total)
        return;
    m_processCur = cur;
    m_processTotal = total;

    QString text = QString().sprintf("%02d:%02d:%02d/%02d:%02d:%02d",
                                     m_processCur     / (24 * 60),
                                     m_processCur     / (60),
                                     m_processCur     % (60),
                                     m_processTotal   / (24 * 60),
                                     m_processTotal   / (60),
                                     m_processTotal   % (60));
    text = QFontMetrics(m_labelText->font()).elidedText(text, Qt::ElideRight, m_labelText->width());
    m_labelText->setText(text);
}

void VideoControlPanel::paintEvent(QPaintEvent *event)
{
    QPainter paint(this);
    QColor c = PropertyColor::subToolColor;
    c.setAlpha(100);
    paint.fillRect(this->rect(), c);
    QWidget::paintEvent(event);
}

void VideoControlPanel::mouseMoveEvent(QMouseEvent *event)
{
    if(m_controlPanelTimer->isActive()) {
        m_controlPanelTimer->stop();
    }
    QWidget::mouseMoveEvent(event);
}

void VideoControlPanel::updateControlPanelState(ControlState state)
{
    switch(state) {
    case VideoPlay:
        m_buttonPlay->hide();
        m_buttonPause->show();
        break;
    case VideoPause:
        m_buttonPlay->show();
        m_buttonPause->hide();
        break;
    case VideoFullScreen:
        m_buttonFullScreen->hide();
        m_buttonFullScreenExit->show();
        break;
    case VideoNormalScreen:
        m_buttonFullScreen->show();
        m_buttonFullScreenExit->hide();
        break;
    }
}
