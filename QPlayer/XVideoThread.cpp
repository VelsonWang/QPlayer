#include <iostream>
#include "XVideoThread.h"
#include "XDecode.h"

using namespace std;

XVideoThread::XVideoThread() {
}

XVideoThread::~XVideoThread() {
}

//打开，不管成功与否都清理
bool XVideoThread::open(AVCodecParameters *para,
                        IVideoCall *call,
                        int width,
                        int height) {
    if (!para)
        return false;
    clear();

    std::unique_lock<std::mutex> lock(vmutex_);
    syncpts_ = 0;
	//初始化显示窗口
    videoCall_ = call;
    if (videoCall_) {
        videoCall_->init(width, height);
    }

    int ret = true;
    if (!decode_->open(para)) {
        ret = false;
	}	
    return ret;
}

void XVideoThread::setPause(bool isPause) {
    std::unique_lock<std::mutex> lock(vmutex_);
    this->isPause_ = isPause;
}

void XVideoThread::run() {
    while (!isExit_) {
        vmutex_.lock();
        if (this->isPause_) {
            vmutex_.unlock();
			msleep(5);
			continue;
		}
        //音视频同步
        if (syncpts_ > 0 && syncpts_ < decode_->pts_) {
            vmutex_.unlock();
			msleep(1);
			continue;
		}
        AVPacket *pkt = pop();
        bool re = decode_->send(pkt);
        if (!re) {
            vmutex_.unlock();
			msleep(1);
			continue;
		}
		//一次send 多次recv
        while (!isExit_) {
            AVFrame * frame = decode_->recv();
			if (!frame)break;
			//显示视频
            if (videoCall_) {
                videoCall_->repaint(frame);
            }
		}
        vmutex_.unlock();
	}
}

//解码pts，如果接收到的解码数据pts >= seekpts return true 并且显示画面
bool XVideoThread::repaintPts(AVPacket *pkt, long long seekpts) {
    std::unique_lock<std::mutex> lock(vmutex_);
    bool re = decode_->send(pkt);
    if (!re) {
        return true; //表示结束解码
	}
    AVFrame *frame = decode_->recv();
    if (!frame) {
        return false;
	}
	//到达位置
    if (decode_->pts_ >= seekpts) {
        if(videoCall_) {
            videoCall_->repaint(frame);
        }
        return true;
	}
    XFreeFrame(&frame);
	return false;
}

