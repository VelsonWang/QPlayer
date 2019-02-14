#include "XSlider.h"

XSlider::XSlider(QWidget *parent)
	: QSlider(parent)
{
}

XSlider::~XSlider()
{
}

void XSlider::mousePressEvent(QMouseEvent *event)
{
    double pos = (double)event->pos().x()/(double)width();
    setValue(pos*maximum());
    //QSlider::mousePressEvent(event);
    QSlider::sliderReleased();
}

