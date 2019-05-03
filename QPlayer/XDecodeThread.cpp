#include "XDecodeThread.h"
#include "XDecode.h"

XDecodeThread::XDecodeThread() {
    // 打开解码器
    if (!decode_) decode_ = new XDecode();
}


XDecodeThread::~XDecodeThread() {
    isExit_ = true;
    wait();
}

//清理资源，停止线程
void XDecodeThread::close() {
    clear();
	
    // 等待线程退出
    isExit_ = true;
	wait();
    decode_->close();

    std::unique_lock<std::mutex> lock(mutex_);
    delete decode_;
    decode_ = NULL;
}

void XDecodeThread::clear() {
    std::unique_lock<std::mutex> lock(mutex_);
    decode_->clear();
    while (!packs_.empty()) {
        AVPacket *pkt = packs_.front();
		XFreePacket(&pkt);
        packs_.pop_front();
    }
}

//取出一帧数据，并出栈，如果没有返回NULL
AVPacket *XDecodeThread::pop() {
    std::unique_lock<std::mutex> lock(mutex_);
    if (packs_.empty()) {
        return NULL;
	}
    AVPacket *pkt = packs_.front();
    packs_.pop_front();
	return pkt;
}

void XDecodeThread::push(AVPacket *pkt) {
    if (!pkt)return;
	//阻塞
    while (!isExit_) {
        mutex_.lock();
        if (packs_.size() < maxList_) {
            packs_.push_back(pkt);
            mutex_.unlock();
			break;
		}
        mutex_.unlock();
		msleep(1);
	}
}



