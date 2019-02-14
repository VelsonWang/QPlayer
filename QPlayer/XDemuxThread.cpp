#include "XDemuxThread.h"
#include "XDemux.h"
#include "XVideoThread.h"
#include "XAudioThread.h"
#include <iostream>

extern "C"
{
#include <libavformat/avformat.h>
}

#include "XDecode.h"

using namespace std;

XDemuxThread::XDemuxThread()
{
}


XDemuxThread::~XDemuxThread()
{
    isExit_ = true;
    wait();
}

void XDemuxThread::clear()
{
    mutex_.lock();
    if (demux_) demux_->clear();
    if (videoThread_) videoThread_->clear();
    if (audioThread_) audioThread_->clear();
    mutex_.unlock();
}

void XDemuxThread::seek(double pos)
{
	//清理缓存
    clear();

    mutex_.lock();
    bool status = this->isPause_;
    mutex_.unlock();
	//暂停
    setPause(true);

    mutex_.lock();
    if (demux_)
        demux_->seek(pos);
	//实际要显示的位置pts
    long long seekPts = pos*demux_->totalMs_;
    while (!isExit_)
	{
        AVPacket *pkt = demux_->readVideo();
		if (!pkt) break;
		//如果解码到seekPts
        if (videoThread_->repaintPts(pkt, seekPts))
		{
            this->pts_ = seekPts;
			break;
		}
	}

    mutex_.unlock();

	//seek是非暂停状态
	if(!status)
        setPause(false);
}

void XDemuxThread::setPause(bool isPause)
{
    mutex_.lock();
    this->isPause_ = isPause;
    if (audioThread_) audioThread_->setPause(isPause);
    if (videoThread_) videoThread_->setPause(isPause);
    mutex_.unlock();
}

void XDemuxThread::run()
{
    while (!isExit_)
	{
        mutex_.lock();
        if (isPause_)
		{
            mutex_.unlock();
			msleep(5);
			continue;
		}
        if (!demux_)
		{
            mutex_.unlock();
			msleep(5);
			continue;
		}

		
		//音视频同步
        if (videoThread_ && audioThread_)
		{
            pts_ = audioThread_->pts_;
            videoThread_->syncpts_ = audioThread_->pts_;
		}

        AVPacket *pkt = demux_->read();
		if (!pkt) 
		{
            mutex_.unlock();
			msleep(5);
			continue;
		}
		//判断数据是音频
        if (demux_->isAudio(pkt))
		{
            if(audioThread_) audioThread_->push(pkt);
		}
		else //视频
		{
            if (videoThread_)videoThread_->push(pkt);
		}
        mutex_.unlock();
		msleep(1);
	}
}


bool XDemuxThread::open(const char *url, IVideoCall *call)
{
	if (url == 0 || url[0] == '\0')
		return false;

    mutex_.lock();
    if (!demux_) demux_ = new XDemux();
    if (!videoThread_) videoThread_ = new XVideoThread();
    if (!audioThread_) audioThread_ = new XAudioThread();

	//打开解封装
    bool ret = demux_->open(url);
    if (!ret)
	{
        mutex_.unlock();
        //cout << "demux->Open(url) failed!" << endl;
		return false;
	}
	//打开视频解码器和处理线程
    if (!videoThread_->open(demux_->copyVPara(), call, demux_->width_, demux_->height_))
	{
        ret = false;
        //cout << "vt->Open failed!" << endl;
	}
	//打开音频解码器和处理线程
    if (!audioThread_->open(demux_->copyAPara(), demux_->sampleRate_, demux_->channels_))
	{
        ret = false;
        //cout << "at->Open failed!" << endl;
	}
    totalMs_ = demux_->totalMs_;
    mutex_.unlock();

    //cout << "XDemuxThread::Open " << re << endl;
    return ret;
}

//关闭线程清理资源
void XDemuxThread::close()
{
    isExit_ = true;
	wait();
    if (videoThread_) videoThread_->close();
    if (audioThread_) audioThread_->close();
    mutex_.lock();
    delete videoThread_;
    delete audioThread_;
    videoThread_ = nullptr;
    audioThread_ = nullptr;
    mutex_.unlock();
}

//启动所有线程
void XDemuxThread::start()
{
    mutex_.lock();
    if (!demux_) demux_ = new XDemux();
    if (!videoThread_) videoThread_ = new XVideoThread();
    if (!audioThread_) audioThread_ = new XAudioThread();
	//启动当前线程
	QThread::start();
    if (videoThread_)videoThread_->start();
    if (audioThread_)audioThread_->start();
    mutex_.unlock();
}

