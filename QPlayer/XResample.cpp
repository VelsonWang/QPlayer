#include "XResample.h"

extern "C" {
#include <libswresample/swresample.h>
#include <libavcodec/avcodec.h>
}

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
    mutex_.lock();
    if (swrContext_) swr_free(&swrContext_);
    mutex_.unlock();
}

/**
 * @brief XResample::open
 * @details 输出参数和输入参数一致除了采样格式，输出为S16
 * @param para
 * @param isClearPara
 * @return
 */
bool XResample::open(AVCodecParameters *para, bool isClearPara)
{
    if (!para) return false;
    mutex_.lock();

    //如果swrContext_为NULL会分配空间
    swrContext_ = swr_alloc_set_opts(swrContext_,
                                     av_get_default_channel_layout(2), //输出格式
                                     (AVSampleFormat)outFormat_, //输出样本格式 1 AV_SAMPLE_FMT_S16
                                     para->sample_rate, //输出采样率
                                     av_get_default_channel_layout(para->channels), //输入格式
                                     (AVSampleFormat)para->format,
                                     para->sample_rate,
                                     0, 0);
    if(isClearPara) avcodec_parameters_free(&para);

    int re = swr_init(swrContext_);
    mutex_.unlock();
	if (re != 0)
	{
		char buf[1024] = { 0 };
		av_strerror(re, buf, sizeof(buf) - 1);
        //cout << "swr_init  failed! :" << buf << endl;
		return false;
	}
	return true;
}


/**
 * @brief XResample::resample
 * @details 返回重采样后大小,不管成功与否都释放indata空间
 * @param indata
 * @param data
 * @return
 */
int XResample::resample(AVFrame *indata, unsigned char *data)
{
    if (!indata) return 0;
    if (!data)
    {
        av_frame_free(&indata);
        return 0;
    }
    uint8_t *dat[2] = { 0 };
    dat[0] = data;
    int ret = swr_convert(swrContext_,
                         dat,
                         indata->nb_samples, // 输出
                         (const uint8_t**)indata->data,
                         indata->nb_samples	// 输入
                         );
    int outSize = ret * indata->channels * av_get_bytes_per_sample((AVSampleFormat)outFormat_);
    av_frame_free(&indata);

    return (ret <= 0)?ret:outSize;
}
