#ifndef CTRLBAR_H
#define CTRLBAR_H

#include <QWidget>
#include "CustomSlider.h"

namespace Ui {
class CtrlBar;
}

class CtrlBar : public QWidget
{
    Q_OBJECT

public:
    explicit CtrlBar(QWidget *parent = Q_NULLPTR);
    ~CtrlBar();
    bool init();
    bool isPlaySliderPressed();

public:
    void onVideoTotalSeconds(int nSeconds);
    void onVideoPlaySeconds(int nSeconds);
    void onVideopVolume(double dPercent);
    void onPauseStat(bool bPaused);
    void onStopFinished();

private:
    void onPlaySliderValueChanged();
    void onVolumeSliderValueChanged();

private slots:
    void on_PlayOrPauseBtn_clicked();
    void on_VolumeBtn_clicked();
    void on_StopBtn_clicked();
    void on_SettingBtn_clicked();
    bool connectSignalSlots();
    void onCustomSliderPressed();
    void onCustomSliderReleased();

signals:
    void sigShowOrHidePlaylist();	// 显示或隐藏信号
    void sigPlaySeek(double dPercent); // 调整播放进度
    void sigPlayVolume(double dPercent);
    void sigPlayOrPause();
    void sigStop();
    void sigForwardPlay();
    void sigBackwardPlay();
    void sigShowMenu();
    void sigShowSetting();

private:
    Ui::CtrlBar *ui;
    int totalPlaySeconds_;
    double lastVolumePercent_;
    bool isPlaySliderPress_;

};

#endif // CTRLBAR_H
