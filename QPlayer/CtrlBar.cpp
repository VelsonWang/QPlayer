#include <QDebug>
#include <QTime>
#include <QSettings>
 
#include "ctrlbar.h"
#include "ui_ctrlbar.h"
#include "GlobalHelper.h"

CtrlBar::CtrlBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CtrlBar)
{
    ui->setupUi(this);
    lastVolumePercent_ = 1.0;
}

CtrlBar::~CtrlBar()
{
    delete ui;
}

bool CtrlBar::init()
{
    setStyleSheet(GlobalHelper::getQssStr(":/Resources/qss/ctrlbar.css"));

    GlobalHelper::setIcon(ui->PlayOrPauseBtn, 12, QChar(0xf04b));
    GlobalHelper::setIcon(ui->StopBtn, 12, QChar(0xf04d));
    GlobalHelper::setIcon(ui->VolumeBtn, 12, QChar(0xf028));
    GlobalHelper::setIcon(ui->PlaylistCtrlBtn, 12, QChar(0xf036));
    GlobalHelper::setIcon(ui->ForwardBtn, 12, QChar(0xf051));
    GlobalHelper::setIcon(ui->BackwardBtn, 12, QChar(0xf048));
    GlobalHelper::setIcon(ui->SettingBtn, 12, QChar(0xf013));

    ui->PlaylistCtrlBtn->setToolTip("播放列表");
    ui->SettingBtn->setToolTip("设置");
    ui->VolumeBtn->setToolTip("静音");
    ui->ForwardBtn->setToolTip("下一个");
    ui->BackwardBtn->setToolTip("上一个");
    ui->StopBtn->setToolTip("停止");
    ui->PlayOrPauseBtn->setToolTip("播放");
    
    connectSignalSlots();

    double dPercent = -1.0;
    GlobalHelper::getPlayVolume(dPercent);
    if (dPercent != -1.0) {
        emit sigPlayVolume(dPercent);
        onVideopVolume(dPercent);
    }

    return true;

}

bool CtrlBar::connectSignalSlots()
{
    connect(ui->PlaylistCtrlBtn, &QPushButton::clicked, this, &CtrlBar::sigShowOrHidePlaylist);
    connect(ui->PlaySlider, &CustomSlider::sigCustomSliderValueChanged, this, &CtrlBar::onPlaySliderValueChanged);
    connect(ui->VolumeSlider, &CustomSlider::sigCustomSliderValueChanged, this, &CtrlBar::onVolumeSliderValueChanged);
    connect(ui->BackwardBtn, &QPushButton::clicked, this, &CtrlBar::sigBackwardPlay);
    connect(ui->ForwardBtn, &QPushButton::clicked, this, &CtrlBar::sigForwardPlay);

    return true;
}

void CtrlBar::onVideoTotalSeconds(int nSeconds)
{
    totalPlaySeconds_ = nSeconds;

    int thh, tmm, tss;
    thh = nSeconds / 3600;
    tmm = (nSeconds % 3600) / 60;
    tss = (nSeconds % 60);
    QTime TotalTime(thh, tmm, tss);

    ui->VideoTotalTimeTimeEdit->setTime(TotalTime);
}


void CtrlBar::onVideoPlaySeconds(int nSeconds)
{
    int thh, tmm, tss;
    thh = nSeconds / 3600;
    tmm = (nSeconds % 3600) / 60;
    tss = (nSeconds % 60);
    QTime TotalTime(thh, tmm, tss);

    ui->VideoPlayTimeTimeEdit->setTime(TotalTime);

    ui->PlaySlider->setValue(nSeconds * 1.0 / totalPlaySeconds_ * MAX_SLIDER_VALUE);
}

void CtrlBar::onVideopVolume(double dPercent)
{
    ui->VolumeSlider->setValue(dPercent * MAX_SLIDER_VALUE);
    lastVolumePercent_ = dPercent;

    if (lastVolumePercent_ == 0) {
        GlobalHelper::setIcon(ui->VolumeBtn, 12, QChar(0xf026));
    } else {
        GlobalHelper::setIcon(ui->VolumeBtn, 12, QChar(0xf028));
    }

    GlobalHelper::savePlayVolume(dPercent);
}

void CtrlBar::onPauseStat(bool bPaused)
{
    qDebug() << "CtrlBar::OnPauseStat" << bPaused;
    if (bPaused) {
        GlobalHelper::setIcon(ui->PlayOrPauseBtn, 12, QChar(0xf04b));
        ui->PlayOrPauseBtn->setToolTip("播放");
    } else {
        GlobalHelper::setIcon(ui->PlayOrPauseBtn, 12, QChar(0xf04c));
        ui->PlayOrPauseBtn->setToolTip("暂停");
    }
}

void CtrlBar::onStopFinished()
{
    ui->PlaySlider->setValue(0);
    QTime StopTime(0, 0, 0);
    ui->VideoTotalTimeTimeEdit->setTime(StopTime);
    ui->VideoPlayTimeTimeEdit->setTime(StopTime);
    GlobalHelper::setIcon(ui->PlayOrPauseBtn, 12, QChar(0xf04b));
    ui->PlayOrPauseBtn->setToolTip("播放");
}

void CtrlBar::onPlaySliderValueChanged()
{
    double dPercent = ui->PlaySlider->value()*1.0 / ui->PlaySlider->maximum();
    emit sigPlaySeek(dPercent);
}

void CtrlBar::onVolumeSliderValueChanged()
{
    double dPercent = ui->VolumeSlider->value()*1.0 / ui->VolumeSlider->maximum();
    emit sigPlayVolume(dPercent);

    onVideopVolume(dPercent);
}

void CtrlBar::on_PlayOrPauseBtn_clicked()
{
    emit sigPlayOrPause();
}

void CtrlBar::on_VolumeBtn_clicked()
{
    if (ui->VolumeBtn->text() == QChar(0xf028)) {
        GlobalHelper::setIcon(ui->VolumeBtn, 12, QChar(0xf026));
        ui->VolumeSlider->setValue(0);
        emit sigPlayVolume(0);
    } else {
        GlobalHelper::setIcon(ui->VolumeBtn, 12, QChar(0xf028));
        ui->VolumeSlider->setValue(lastVolumePercent_ * MAX_SLIDER_VALUE);
        emit sigPlayVolume(lastVolumePercent_);
    }

}

void CtrlBar::on_StopBtn_clicked()
{
    emit sigStop();
}

void CtrlBar::on_SettingBtn_clicked()
{
    //emit SigShowSetting();
}
