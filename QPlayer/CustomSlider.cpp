#include "CustomSlider.h"
#include "globalhelper.h"

CustomSlider::CustomSlider(QWidget *parent)
    : QSlider(parent)
{
    this->setMaximum(MAX_SLIDER_VALUE);
}

CustomSlider::~CustomSlider()
{
}

void CustomSlider::mousePressEvent(QMouseEvent *ev)
{
    //注意应先调用父类的鼠标点击处理事件，这样可以不影响拖动的情况
    QSlider::mousePressEvent(ev);
    double pos = ev->pos().x() / (double)width();
    setValue(pos * (maximum() - minimum()) + minimum());

    emit sigCustomSliderValueChanged();
    emit sigCustomSliderPressed();
}

void CustomSlider::mouseReleaseEvent(QMouseEvent *ev)
{
    QSlider::mouseReleaseEvent(ev);

    //emit SigCustomSliderValueChanged();
    emit sigCustomSliderReleased();
}

void CustomSlider::mouseMoveEvent(QMouseEvent *ev)
{
    QSlider::mouseMoveEvent(ev);
    double pos = ev->pos().x() / (double)width();
    setValue(pos * (maximum() - minimum()) + minimum());

    emit sigCustomSliderValueChanged();
}
