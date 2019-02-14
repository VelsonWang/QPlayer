#include "XDecodeThread.h"
#include "XDecode.h"

XDecodeThread::XDecodeThread()
{
    // 打开解码器
    if (!decode_) decode_ = new XDecode();
}


XDecodeThread::~XDecodeThread()
{
    isExit_ = true;
    wait();
}

//清理资源，停止线程
void XDecodeThread::close()
{
    clear();
	
    // 等待线程退出
    isExit_ = true;
	wait();
    decode_->close();

    mutex_.lock();
    delete decode_;
    decode_ = NULL;
    mutex_.unlock();
}

void XDecodeThread::clear()
{
    mutex_.lock();
    decode_->clear();
    while (!packs_.empty())
	{
        AVPacket *pkt = packs_.front();
		XFreePacket(&pkt);
        packs_.pop_front();
	}

    mutex_.unlock();
}

//取出一帧数据，并出栈，如果没有返回NULL
AVPacket *XDecodeThread::pop()
{
    mutex_.lock();
    if (packs_.empty())
	{
        mutex_.unlock();
		return NULL;
	}
    AVPacket *pkt = packs_.front();
    packs_.pop_front();
    mutex_.unlock();
	return pkt;
}

void XDecodeThread::push(AVPacket *pkt)
{
	if (!pkt)return;
	//阻塞
    while (!isExit_)
	{
        mutex_.lock();
        if (packs_.size() < maxList_)
		{
            packs_.push_back(pkt);
            mutex_.unlock();
			break;
		}
        mutex_.unlock();
		msleep(1);
	}
}



