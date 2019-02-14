
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

    virtual void close()
	{
        mutex_.lock();
        if (io_)
		{
            io_->close ();
            io_ = nullptr;
		}
        if (output_)
		{
            output_->stop();
            delete output_;
            output_ = nullptr;
		}
        mutex_.unlock();
	}

    virtual bool open()
	{
        close();
		QAudioFormat fmt;
		fmt.setSampleRate(sampleRate);
		fmt.setSampleSize(sampleSize);
		fmt.setChannelCount(channels);
		fmt.setCodec("audio/pcm");
		fmt.setByteOrder(QAudioFormat::LittleEndian);
		fmt.setSampleType(QAudioFormat::UnSignedInt);
        mutex_.lock();
        output_ = new QAudioOutput(fmt);
        io_ = output_->start(); //开始播放
        mutex_.unlock();
        if(io_) return true;
		return false;
	}

    void clear()
    {
        mutex_.lock();
        if (io_) io_->reset();
        mutex_.unlock();
    }

    virtual bool write(const unsigned char *data, int datasize)
    {
        if (!data || datasize <= 0) return false;
        mutex_.lock();
        if (!output_ || !io_)
        {
            mutex_.unlock();
            return false;
        }
        int size = io_->write((char *)data, datasize);
        mutex_.unlock();
        if (datasize != size) return false;
        return true;
    }

    virtual int getFree()
    {
        mutex_.lock();
        if (!output_)
        {
            mutex_.unlock();
            return 0;
        }
        int free = output_->bytesFree();
        mutex_.unlock();
        return free;
    }

    virtual long long getNoPlayMs()
    {
        mutex_.lock();
        if (!output_)
        {
            mutex_.unlock();
            return 0;
        }
        long long pts = 0;
        //还未播放的字节数
        double size = output_->bufferSize() - output_->bytesFree();
        //一秒音频字节大小
        double secSize = sampleRate*(sampleSize/8)*channels;
        if (secSize <= 0) pts = 0;
        else pts = (size/secSize)*1000;

        mutex_.unlock();
        return pts;
    }

    void setPause(bool isPause)
    {
        mutex_.lock();
        if (!output_)
        {
            mutex_.unlock();
            return;
        }
        if (isPause) output_->suspend();
        else output_->resume();
        mutex_.unlock();
    }
};


XAudioPlay *XAudioPlay::get()
{
	static CXAudioPlay play;
	return &play;
}

XAudioPlay::XAudioPlay()
{
}


XAudioPlay::~XAudioPlay()
{
}
