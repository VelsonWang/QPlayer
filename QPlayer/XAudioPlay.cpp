
#include "XAudioPlay.h"
#include <QAudioFormat>
#include <QAudioOutput>
#include <mutex>

class CXAudioPlay : public XAudioPlay
{
public:
    QAudioOutput *output_ = NULL;
    QIODevice *io_ = NULL;
    std::mutex mutex_;

    virtual void close()
	{
        mutex_.lock();
        if (io_)
		{
            io_->close ();
            io_ = NULL;
		}
        if (output_)
		{
            output_->stop();
            delete output_;
            output_ = 0;
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
        if(io_)
			return true;
		return false;
	}

    virtual bool write(const unsigned char *data, int datasize)
    {
        if (!data || datasize <= 0)return false;
        mutex_.lock();
        if (!output_ || !io_)
        {
            mutex_.unlock();
            return false;
        }
        int size = io_->write((char *)data, datasize);
        mutex_.unlock();
        if (datasize != size)
            return false;
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
