
#include "XPlay2.h"
#include "XDemuxThread.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>

static XDemuxThread demuxThread;

XPlay2::XPlay2(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
    connect(ui.openFile, SIGNAL(clicked(bool)), this, SLOT(openFile()));
    connect(ui.isplay, SIGNAL(clicked(bool)), this, SLOT(playOrPause()));
    connect(ui.playPos, SIGNAL(sliderPressed()), this, SLOT(sliderPress()));
    connect(ui.playPos, SIGNAL(sliderReleased()), this, SLOT(sliderRelease()));

    demuxThread.start();
    startTimer(40);
}

XPlay2::~XPlay2()
{
    demuxThread.close();
}


void XPlay2::sliderPress()
{
    isSliderPress_ = true;
}

void XPlay2::sliderRelease()
{
    isSliderPress_ = false;
    double pos = 0.0;
    pos = (double)ui.playPos->value()/(double)ui.playPos->maximum();
    demuxThread.seek(pos);
}

//定时器 滑动条显示
void XPlay2::timerEvent(QTimerEvent *e)
{
    if (isSliderPress_) return;
    long long total = demuxThread.totalMs_;
    if (total > 0)
    {
        double pos = (double)demuxThread.pts_/(double)total;
        int v = ui.playPos->maximum() * pos;
        ui.playPos->setValue(v);
    }
}

//双击全屏
void XPlay2::mouseDoubleClickEvent(QMouseEvent *e)
{
    if (isFullScreen()) this->showNormal();
    else this->showFullScreen();
}

//窗口尺寸变化
void XPlay2::resizeEvent(QResizeEvent *e)
{
    ui.playPos->move(50, this->height() - 100);
    ui.playPos->resize(this->width() - 100, ui.playPos->height());
    ui.openFile->move(100,this->height() - 150);
    ui.isplay->move(ui.openFile->x() + ui.openFile->width() + 10, ui.openFile->y());
    ui.video->resize(this->size());
}

void XPlay2::playOrPause()
{
    bool isPause = !demuxThread.isPause_;
    setPause(isPause);
    demuxThread.setPause(isPause);
}

void XPlay2::setPause(bool isPause)
{
    if (isPause) ui.isplay->setText(QString::fromLocal8Bit("播 放"));
    else ui.isplay->setText(QString::fromLocal8Bit("暂 停"));
}



void XPlay2::openFile()
{
    QString name = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择视频文件"));
    if (name.isEmpty())return;
    this->setWindowTitle(name);
    if (!demuxThread.open(name.toLocal8Bit(), ui.video))
    {
        QMessageBox::information(0, "error", "open file failed!");
        return;
    }
    setPause(demuxThread.isPause_);
}
