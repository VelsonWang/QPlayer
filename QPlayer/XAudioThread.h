
#ifndef XAUDIOTHREAD_H
#define XAUDIOTHREAD_H

#include <QThread>
#include <mutex>
#include <list>
#include "XDecodeThread.h"

struct AVCodecParameters;
class XAudioPlay;
class XResample;


class XAudioThread : public XDecodeThread
{
public:
    XAudioThread();
    virtual ~XAudioThread();

    // 打开
    virtual bool open(AVCodecParameters *para, int sampleRate, int channels);

    // 停止线程，清理资源
    virtual void close();

    virtual void clear();
	void run();

    void setPause(bool isPause);
    // 设置音量
    void setVolume(float volume);

public:
    bool isPause_ = false;
    // 当前音频播放的pts
    long long pts_ = 0;

protected:
    std::mutex amutex_;
    XAudioPlay *audioPlay_ = 0;
    XResample *resample_ = 0;

};

#endif


