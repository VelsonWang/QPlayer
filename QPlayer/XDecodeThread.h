#ifndef XDECODETHREAD_H
#define XDECODETHREAD_H

#include <list>
#include <mutex>
#include <QThread>

///解码和显示视频
struct AVPacket;
class XDecode;

class XDecodeThread : public QThread
{
public:
	XDecodeThread();
	virtual ~XDecodeThread();

    virtual void push(AVPacket *pkt);

	//清理队列
    virtual void clear();

	//清理资源，停止线程
    virtual void close();

	//取出一帧数据，并出栈，如果没有返回NULL
    virtual AVPacket *pop();

public:	
    int maxList_ = 100; // 最大队列
    bool isExit_ = false;

protected:
    XDecode *decode_ = 0;
    std::list <AVPacket *> packs_;
    std::mutex mutex_;
};

#endif


