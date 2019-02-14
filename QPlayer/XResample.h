#ifndef XRESAMPLLE_H
#define XRESAMPLLE_H

#include <mutex>

struct AVCodecParameters;
struct SwrContext;
struct AVFrame;


class XResample
{
public:
    XResample();
    ~XResample();

    virtual bool open(AVCodecParameters *para, bool isClearPara = false);
    virtual void close();
    virtual int resample(AVFrame *indata, unsigned char *data);

public:
    int outFormat_ = 1; // AV_SAMPLE_FMT_S16

protected:
    std::mutex mutex_;
    SwrContext *swrContext_ = 0;
};

#endif



