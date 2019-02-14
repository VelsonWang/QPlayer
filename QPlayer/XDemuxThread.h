#ifndef XDEMUXTHREAD_H
#define XDEMUXTHREAD_H

#include <QThread>
#include "IVideoCall.h"
#include <mutex>

class XDemux;
class XVideoThread;
class XAudioThread;

class XDemuxThread : public QThread
{
public:
    XDemuxThread();
    virtual ~XDemuxThread();

	//创建对象并打开
    virtual bool open(const char *url, IVideoCall *call);

	//启动所有线程
    virtual void start();

	//关闭线程清理资源
    virtual void close();
    virtual void clear();

    virtual void seek(double pos);

	void run();
    void setPause(bool isPause);

public:
    bool isExit_ = false;
    long long pts_ = 0;
    long long totalMs_ = 0;
    bool isPause_ = false;

protected:
    std::mutex mutex_;
    XDemux *demux_ = 0;
    XVideoThread *videoThread_ = nullptr;
    XAudioThread *audioThread_ = nullptr;
};

#endif


