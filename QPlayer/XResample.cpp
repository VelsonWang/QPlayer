#include "XResample.h"

extern "C" {
#include <libswresample/swresample.h>
#include <libavcodec/avcodec.h>
}

#pragma comment(lib, "swresample.lib")
#include <iostream>

using namespace std;

XResample::XResample()
{
}


XResample::~XResample()
{
}

void XResample::close()
{
	mux.lock();
	if (actx)
		swr_free(&actx);

	mux.unlock();
}

//输出参数和输入参数一致除了采样格式，输出为S16
bool XResample::open(AVCodecParameters *para)
{
	if (!para)return false;
	mux.lock();
	//音频重采样 上下文初始化
	//if(!actx)
	//	actx = swr_alloc();

	//如果actx为NULL会分配空间
	actx = swr_alloc_set_opts(actx,
		av_get_default_channel_layout(2),	//输出格式
		AV_SAMPLE_FMT_S16,					//输出样本格式
		para->sample_rate,					//输出采样率
		av_get_default_channel_layout(para->channels),//输入格式
		(AVSampleFormat)para->format,
		para->sample_rate,
		0, 0
	);
	int re = swr_init(actx);
	mux.unlock();
	if (re != 0)
	{
		char buf[1024] = { 0 };
		av_strerror(re, buf, sizeof(buf) - 1);
		cout << "swr_init  failed! :" << buf << endl;
		return false;
	}
	//unsigned char *pcm = NULL;
	return true;
}

//返回重采样后大小,不管成功与否都释放indata空间
int XResample::resample(AVFrame *indata, unsigned char *d)
{
    if (!indata) return 0;
    if (!d)
    {
        av_frame_free(&indata);
        return 0;
    }
    uint8_t *data[2] = { 0 };
    data[0] = d;
    int re = swr_convert(actx,
                         data,
                         indata->nb_samples, // 输出
                         (const uint8_t**)indata->data,
                         indata->nb_samples	// 输入
                         );
    if (re <= 0)return re;
    int outSize = re * indata->channels * av_get_bytes_per_sample((AVSampleFormat)outFormat);
    return outSize;
}
