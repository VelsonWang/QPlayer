#ifndef XPLAY2_H
#define XPLAY2_H

#include <QTimerEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QWidget>
#include "ui_XPlay2.h"

class XPlay2 : public QWidget
{
	Q_OBJECT

public:
	XPlay2(QWidget *parent = Q_NULLPTR);
    ~XPlay2();

    //定时器 滑动条显示
    void timerEvent(QTimerEvent *e);

    //窗口尺寸变化
    void resizeEvent(QResizeEvent *e);

    //双击全屏
    void mouseDoubleClickEvent(QMouseEvent *e);
    void setPause(bool isPause);

public slots:
    void openFile();
    void playOrPause();
    void sliderPress();
    void sliderRelease();

private:
    bool isSliderPress_ = false;
    Ui::XPlay2Class ui;

};


#endif


