
#include "XAudioPlay.h"
#include <QAudioFormat>
#include <QAudioOutput>
#include <mutex>
#include <cmath>
#include <QDebug>


void raiseVolume(unsigned char* buf, int size, float vol)
{
    for (int i = 0; i < size; i += 2) {
        short wData = (unsigned short)(buf[i] | (buf[i + 1] << 8));
        long dwData = wData;
        dwData = dwData * vol;
        if (dwData < -0x8000)
        {
            dwData = -0x8000;
        }
        else if (dwData > 0x7FFF)
        {
            dwData = 0x7FFF;
        }

        wData = dwData & 0xFFFF;
        buf[i] = wData & 0xFF;
        buf[i + 1] = (wData & 0xFF00) >> 8;
    }
}


class CXAudioPlay : public XAudioPlay
{
public:
    QAudioOutput *output_ = nullptr;
    QIODevice *io_ = nullptr;
    std::mutex mutex_;

    virtual void close() {
        std::unique_lock<std::mutex> lock(mutex_);
        if (io_) {
            io_->close ();
            io_ = nullptr;
		}
        if (output_) {
            output_->stop();
            delete output_;
            output_ = nullptr;
		}
	}

    virtual bool open() {
        close();
		QAudioFormat fmt;
		fmt.setSampleRate(sampleRate);
		fmt.setSampleSize(sampleSize);
		fmt.setChannelCount(channels);
		fmt.setCodec("audio/pcm");
		fmt.setByteOrder(QAudioFormat::LittleEndian);
		fmt.setSampleType(QAudioFormat::UnSignedInt);
        std::unique_lock<std::mutex> lock(mutex_);
        output_ = new QAudioOutput(fmt);
        output_->setVolume(1.0);
        io_ = output_->start(); //开始播放
        lock.unlock();
        if(io_) return true;
		return false;
	}

    void clear() {
        std::unique_lock<std::mutex> lock(mutex_);
        if (io_) {
            io_->reset();
        }
    }

    virtual bool write(const unsigned char *data, int datasize) {
        if (!data || datasize <= 0)
            return false;
        std::unique_lock<std::mutex> lock(mutex_);
        if (!output_ || !io_) {
            return false;
        }

        unsigned char *pBuf = new unsigned char[datasize];
        memset(pBuf, 0, static_cast<size_t>(datasize));
        memcpy(pBuf, data, static_cast<size_t>(datasize));
        raiseVolume(pBuf, datasize, fVolume_);
        int size = io_->write((const char *)pBuf, datasize);
        delete[] pBuf;

        return datasize == size;
    }

    virtual int getFree() {
        std::unique_lock<std::mutex> lock(mutex_);
        if (!output_) {
            return 0;
        }
        int free = output_->bytesFree();
        return free;
    }

    virtual long long getNoPlayMs() {
        std::unique_lock<std::mutex> lock(mutex_);
        if (!output_) {
            return 0;
        }
        long long pts = 0;
        //还未播放的字节数
        double size = output_->bufferSize() - output_->bytesFree();
        //一秒音频字节大小
        double secSize = sampleRate*(sampleSize/8)*channels;
        if (secSize <= 0) pts = 0;
        else pts = (size/secSize)*1000;

        return pts;
    }

    void setPause(bool isPause) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (!output_) {
            return;
        }
        if (isPause) {
            output_->suspend();
        }
        else {
            output_->resume();
        }
    }

    // 设置音量
    void setVolume(float volume) {
        /*
        if (!output_) {
            return;
        }
        output_->setVolume(static_cast<qreal>(volume)); */
        fVolume_ = volume;
    }
};


XAudioPlay *XAudioPlay::get() {
	static CXAudioPlay play;
	return &play;
}

XAudioPlay::XAudioPlay() {
}


XAudioPlay::~XAudioPlay() {
}
