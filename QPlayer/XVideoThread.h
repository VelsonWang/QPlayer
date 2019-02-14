#ifndef XVIDEOTHREAD_H
#define XVIDEOTHREAD_H

#include <list>
#include <mutex>
#include <QThread>
#include "IVideoCall.h"
#include "XDecodeThread.h"

///解码和显示视频
struct AVPacket;
struct AVCodecParameters;
class XDecode;

class XVideoThread : public XDecodeThread
{
public:
    XVideoThread();
    virtual ~XVideoThread();

	//解码pts，如果接收到的解码数据pts >= seekpts return true 并且显示画面
    virtual bool repaintPts(AVPacket *pkt, long long seekpts);
	//打开，不管成功与否都清理
    virtual bool open(AVCodecParameters *para, IVideoCall *call, int width, int height);
	void run();
    void setPause(bool isPause);

public:
	//同步时间，由外部传入
    long long syncpts_ = 0;
    bool isPause_ = false;

protected:
    IVideoCall *videoCall_ = 0;
    std::mutex vmutex_;


};

#endif


