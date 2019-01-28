#ifndef XAUDIOPLAY
#define XAUDIOPLAY


class XAudioPlay
{
public:
    XAudioPlay();
    virtual ~XAudioPlay();

    //打开音频播放
    virtual bool open() = 0;
    virtual void close() = 0;

    //播放音频
    virtual bool write(const unsigned char *data, int datasize) = 0;
    virtual int getFree() = 0;

    static XAudioPlay *get();

public:
	int sampleRate = 44100;
	int sampleSize = 16;
	int channels = 2;
	


};

#endif
