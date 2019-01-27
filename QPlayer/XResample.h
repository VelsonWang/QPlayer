#pragma once
struct AVCodecParameters;
struct SwrContext;
#include <mutex>

class XResample
{
public:
    XResample();
    ~XResample();

	//输出参数和输入参数一致除了采样格式，输出为S16
    virtual bool open(AVCodecParameters *para);
    virtual void close();


protected:
	std::mutex mux;
	SwrContext *actx = 0;
};

