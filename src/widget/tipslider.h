#ifndef TIPSLIDER_H
#define TIPSLIDER_H

#include <QSlider>
#include <QLabel>

class TipSlider : public QSlider
{
    Q_OBJECT
public:
    explicit TipSlider(QWidget *parent = 0);
    explicit TipSlider(Qt::Orientation orientation, QWidget *parent = 0);

public:
    void setTipValue(int value);
public slots:
    void updateTipPos();
protected:
    virtual void sliderChange(SliderChange change);
private:
    QLabel *tip;
};

#endif // TIPSLIDER_H
