#include "videocontrolpanel.h"
#include "lds.h"
#include "videocell.h"
#include "propertycolor.h"

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
    m_buttonPlay            = createPanelButton("buttonIcon", QSize(15, 15), 0x25b6);
    m_buttonPause           = createPanelButton("buttonIcon", QSize(15, 15), 0x23f8);
    m_buttonClose           = createPanelButton("buttonIcon", QSize(15, 15), 0x23f9);
    m_buttonFullScreen      = createPanelButton("buttonIcon", QSize(15, 15), 0xf424);
    m_buttonFullScreenExit  = createPanelButton("buttonIcon", QSize(15, 15), 0xf422);
    m_buttonFullScreenExit->hide();

    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->addWidget(m_labelText, 1);
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
}


QPushButton * VideoControlPanel::createPanelButton(const QString &objectName, const QSize &size, QChar icon) {

    QPushButton *b = new QPushButton;
    b->setFocusPolicy(Qt::NoFocus);
    b->setFlat(true);
    b->setIcon(lds::getFontPixmap(icon, PropertyColor::buttonTextColor, size));
    b->setObjectName(objectName);
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
