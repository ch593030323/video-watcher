#include "cameracontrolpanel.h"
#include "propertycolor.h"

//获取正方形的rect
static QRectF getSquareRect(QPointF center, qreal radius) {
    return QRectF(center.x() - radius, center.y() - radius, radius * 2, radius *2);
}
//获取圆形的path
static QPainterPath getCirclePath(QPointF center, qreal radius){
    QPainterPath p;
    p.addEllipse(getSquareRect(center, radius));
    return p;
}
//获取pie的path
static QPainterPath getPiePath(QPointF center, qreal r1, qreal r2, qreal startAngle, qreal sweepLength) {

    qreal targetAngle = startAngle + sweepLength;
    qreal PI_targetAngle = targetAngle / 180.0 * M_PI;
    qreal PI_startAngle = startAngle / 180.0 * M_PI;

    QPainterPath path;
    path.moveTo(r2 * cos(PI_startAngle),            -r2 * sin(PI_startAngle));
    path.arcTo(getSquareRect(QPointF(0, 0), r2),    startAngle, sweepLength);
    path.lineTo(r1 * cos(PI_targetAngle),           -r1 * sin(PI_targetAngle));
    path.arcTo(getSquareRect(QPointF(0, 0), r1),    targetAngle, -sweepLength);
    path.closeSubpath();

    path.translate(center);

    return path;
}

CameraControlPanel::CameraControlPanel(QWidget *parent)
    : QWidget(parent)
{
}

void CameraControlPanel::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);

    m_paintInfo.draw(&painter);
}

void CameraControlPanel::mousePressEvent(QMouseEvent *event)
{
    m_paintInfo.isPressed = true;
    m_paintInfo.pressedPoint = event->pos();
    update();
}

void CameraControlPanel::mouseReleaseEvent(QMouseEvent *event)
{
    m_paintInfo.isPressed = false;
    update();
    for(int k = TopRight; k <= Center; k ++) {
        if(m_paintInfo.pathMap[k].contains(event->pos())) {
            emit clicked((Direction)k);        }
    }
}

void CameraControlPanel::resizeEvent(QResizeEvent *event)
{
    m_paintInfo.update(this->rect());
}

void CameraControlPanel::PaintInfo::update(QRect rect)
{
    //可控变量
    bgColor = PropertyColor::buttonColor;
    highlightColor = PropertyColor::highlightColor;
    this->width = qMin(rect.width(), rect.height());

    radius = this->width / 2;
    gap = radius / 5;
    radius_inner = radius / 2 - gap;
    pixmapWidth = radius / 3;
    pixmapWidthInner = radius_inner;
    //
    sweepLength = 360.0 / 8;
    offAngle = sweepLength / 2.0;
    startAngle = offAngle;

    qreal xoff = (rect.width() - width) / 2.0;
    qreal yoff = (rect.height() - width) / 2.0;
    //draw pie
    for(int k = TopRight; k <= Right; k ++) {
        qreal start = startAngle + sweepLength * k;
        qreal sweep = sweepLength;

        //painter为Antialiasing模式时，会有白线，略微增加start、start可解决
        QPainterPath path = getPiePath(QPointF(xoff + radius, yoff + radius), radius/2, radius, start - 0.3, sweep + 0.6);
        pathMap[k]= path;
    }

    //draw circle
    {
        QPainterPath path = getCirclePath(QPointF(xoff + radius, yoff + radius), radius_inner);
        pathMap[Center]= path;
    }
}

CameraControlPanel::PaintInfo::PaintInfo()
{
    isPressed = false;
}

void CameraControlPanel::PaintInfo::draw(QPainter *painter)
{
    //draw pie
    for(int k = TopRight; k <= Right; k ++) {
        qreal start = startAngle + sweepLength * k;
        qreal sweep = sweepLength;

        //painter为Antialiasing模式时，会有白线，略微增加start、start可解决
        QPainterPath path = pathMap[k];
        QColor curColor =  (isPressed && path.contains(pressedPoint)) ? highlightColor: bgColor;
        painter->fillPath(path, curColor);

        QPixmap pixmap = PropertyColor::getFontPixmap(0xf0da, PropertyColor::buttonTextColor, QSize(pixmapWidth, pixmapWidth), (start + offAngle) * -1);
        painter->drawPixmap(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, pixmap.size(), path.boundingRect().toRect()),
                           pixmap);
    }

    //draw circle
    {
        QPainterPath path = pathMap[Center];
        QColor curColor =  (isPressed && path.contains(pressedPoint)) ? highlightColor: bgColor;
        painter->fillPath(path, curColor);

        QRect rect = path.boundingRect().toRect();
        QPixmap pixmap = PropertyColor::getFontPixmap(0xf2f9, PropertyColor::buttonTextColor, QSize(pixmapWidthInner, pixmapWidthInner));
        painter->drawPixmap(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, pixmap.size(), rect),
                           pixmap);
    }
}
