
#include "XAudioPlay.h"
#include <QAudioFormat>
#include <QAudioOutput>
#include <mutex>

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
        int size = io_->write((char *)data, datasize);
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
};


XAudioPlay *XAudioPlay::get() {
	static CXAudioPlay play;
	return &play;
}

XAudioPlay::XAudioPlay() {
}


XAudioPlay::~XAudioPlay() {
}
