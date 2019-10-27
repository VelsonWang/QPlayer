#include <iostream>
#include "XAudioThread.h"
#include "XDecode.h"
#include "XAudioPlay.h"
#include "XResample.h"

using namespace std;

XAudioThread::XAudioThread() {
    if (!resample_)
        resample_ = new XResample();
    if (!audioPlay_)
        audioPlay_ = XAudioPlay::get();
}


XAudioThread::~XAudioThread() {
    isExit_ = true;
    wait();
}

void XAudioThread::clear() {
    XDecodeThread::clear();
    std::unique_lock<std::mutex> lock(mutex_);
    if (audioPlay_)
        audioPlay_->clear();
}

//停止线程，清理资源
void XAudioThread::close() {
    XDecodeThread::close();
    if (resample_) {
        resample_->close();
        amutex_.lock();
        delete resample_;
        resample_ = nullptr;
        amutex_.unlock();
	}
    if (audioPlay_) {
        audioPlay_->close();
        amutex_.lock();
        audioPlay_ = nullptr;
        amutex_.unlock();
	}
}

bool XAudioThread::open(AVCodecParameters *para,
                        int sampleRate,
                        int channels) {
    if (!para)
        return false;
    clear();

    std::unique_lock<std::mutex> lock(amutex_);
    pts_ = 0;
    bool ret = true;
    if (!resample_->open(para, false)) {
        ret = false;
	}

    audioPlay_->sampleRate = sampleRate;
    audioPlay_->channels = channels;

    if (!audioPlay_->open()) {
        ret = false;
	}

    if (!decode_->open(para)) {
        ret = false;
	}

    return ret;
}

void XAudioThread::setPause(bool isPause) {
    this->isPause_ = isPause;
    if (audioPlay_)
        audioPlay_->setPause(isPause);
}


/**
 * @brief XAudioThread::setVolume
 * @details 设置音量
 * @param volume
 */
void XAudioThread::setVolume(float volume) {
    if (audioPlay_)
        audioPlay_->setVolume(volume);
}

void XAudioThread::run() {
	unsigned char *pcm = new unsigned char[1024 * 1024 * 10];
    while (!isExit_) {
        amutex_.lock();
        if (isPause_) {
            amutex_.unlock();
			msleep(5);
			continue;
		}

        AVPacket *pkt = pop();
        bool ret = decode_->send(pkt);
        if (!ret) {
            amutex_.unlock();
			msleep(1);
			continue;
		}
		//一次send 多次recv
        while (!isExit_) {
            AVFrame * frame = decode_->recv();
			if (!frame) break;

			//减去缓冲中未播放的时间
            pts_ = decode_->pts_ - audioPlay_->getNoPlayMs();

			//重采样 
            int size = resample_->resample(frame, pcm);
			//播放音频
            while (!isExit_) {
                if (size <= 0)
                    break;
				//缓冲未播完，空间不够
                if (audioPlay_->getFree() < size || isPause_) {
					msleep(1);
					continue;
				}
                audioPlay_->write(pcm, size);
				break;
			}
		}
        amutex_.unlock();
	}
    delete[] pcm;
}


