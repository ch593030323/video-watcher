#ifndef CAMERACONTROLPANEL_H
#define CAMERACONTROLPANEL_H

#include <QWidget>
#include <QtWidgets>

class CameraControlPanel : public QWidget
{
    Q_OBJECT
public:
    static QString IconFontFamily;
    enum Direction {
        TopRight,
        Top,
        LeftTop,
        Left,
        LeftBottom,
        Bottom,
        BottomRight,
        Right,
        Center,
    };
    struct PaintInfo {
        PaintInfo();
        //绘制面板
        void draw(QPainter *painter);

        //更新参数
        void update(QRect rect);//int width, int height);

        //下面变量通过upadte获取
        QColor bgColor;//背景色
        QColor mouseColor;
        qreal width;//矩形画布边长

        qreal radius;//外圈半径
        qreal gap;//间隔
        qreal radius_inner;//内圈半径
        int pixmapWidth;//外圈图标大小
        int pixmapWidthInner;//内圈图片大小

        qreal sweepLength;//偏移的角度
        qreal offAngle;//图标的旋转会用到
        qreal startAngle;//初始位置
        QMap<int, QPainterPath> pathMap;//保存TopRight到Center的path

        //下面变量通过点击事件获取
        bool isPressed;//是否按下
        QPoint pressedPoint;//按下位置
    };

public:
    explicit CameraControlPanel(QWidget *parent = nullptr);

signals:
    void clicked(Direction direct);

protected:
    virtual void paintEvent(QPaintEvent *event);

    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    virtual void resizeEvent(QResizeEvent *event);

    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
private:
    PaintInfo m_paintInfo;
};

#endif // CAMERACONTROLPANEL_H
