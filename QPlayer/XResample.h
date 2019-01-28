#pragma once
struct AVCodecParameters;
struct SwrContext;
struct AVFrame;
#include <mutex>

class XResample
{
public:
    XResample();
    ~XResample();

	//输出参数和输入参数一致除了采样格式，输出为S16
    virtual bool open(AVCodecParameters *para);
    virtual void close();
    //返回重采样后大小,不管成功与否都释放indata空间
    virtual int resample(AVFrame *indata, unsigned char *data);

public:
    //AV_SAMPLE_FMT_S16
    int outFormat = 1;

protected:
	std::mutex mux;
	SwrContext *actx = 0;
};

