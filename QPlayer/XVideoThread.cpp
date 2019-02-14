#include <iostream>
#include "XVideoThread.h"
#include "XDecode.h"

using namespace std;

XVideoThread::XVideoThread()
{
}

XVideoThread::~XVideoThread()
{
}

//打开，不管成功与否都清理
bool XVideoThread::open(AVCodecParameters *para, IVideoCall *call, int width, int height)
{
    if (!para) return false;
    clear();

    vmutex_.lock();
    syncpts_ = 0;
	//初始化显示窗口
    videoCall_ = call;
    if (videoCall_) videoCall_->init(width, height);
    vmutex_.unlock();
    int ret = true;
    if (!decode_->open(para))
	{
        //cout << "audio XDecode open failed!" << endl;
        ret = false;
	}	
    //cout << "XAudioThread::Open :" << ret << endl;
    return ret;
}

void XVideoThread::setPause(bool isPause)
{
    vmutex_.lock();
    this->isPause_ = isPause;
    vmutex_.unlock();
}

void XVideoThread::run()
{
    while (!isExit_)
	{
        vmutex_.lock();
        if (this->isPause_)
		{
            vmutex_.unlock();
			msleep(5);
			continue;
		}
        //cout << "syncpts_ = " << syncpts_ << " dpts =" << decode->pts << endl;
		//音视频同步
        if (syncpts_ > 0 && syncpts_ < decode_->pts_)
		{
            vmutex_.unlock();
			msleep(1);
			continue;
		}
        AVPacket *pkt = pop();
        bool re = decode_->send(pkt);
		if (!re)
		{
            vmutex_.unlock();
			msleep(1);
			continue;
		}
		//一次send 多次recv
        while (!isExit_)
		{
            AVFrame * frame = decode_->recv();
			if (!frame)break;
			//显示视频
            if (videoCall_) videoCall_->repaint(frame);
		}
        vmutex_.unlock();
	}
}

//解码pts，如果接收到的解码数据pts >= seekpts return true 并且显示画面
bool XVideoThread::repaintPts(AVPacket *pkt, long long seekpts)
{
    vmutex_.lock();
    bool re = decode_->send(pkt);
	if (!re)
	{
        vmutex_.unlock();
		return true; //表示结束解码
	}
    AVFrame *frame = decode_->recv();
	if (!frame)
	{
        vmutex_.unlock();
		return false;
	}
	//到达位置
    if (decode_->pts_ >= seekpts)
	{
        if(videoCall_) videoCall_->repaint(frame);
        vmutex_.unlock();
		return true;
	}
	XFreeFrame(&frame);
    vmutex_.unlock();
	return false;
}

