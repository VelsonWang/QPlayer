#include "XDemux.h"
#include <iostream>
#include <thread>

#ifdef __cplusplus
extern "C"{
#endif

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavfilter/avfilter.h>
#include "libavutil/pixfmt.h"
#include "libavutil/frame.h"
#include <libswscale/swscale.h>

#ifdef __cplusplus
}
#endif

using namespace std;

static double r2d(AVRational r)
{
    return r.den == 0 ? 0:(double)r.num / (double)r.den;
}

void XSleep(int ms)
{
    //c++ 11
    chrono::milliseconds du(ms);
    this_thread::sleep_for(du);
}

XDemux::XDemux()
{
    static bool inInit = false;
    static std::mutex dmux;
    dmux.lock();

    if(!inInit)
    {
        //初始化封装库
        av_register_all();

        //初始化网络库 （可以打开rtsp rtmp http 协议的流媒体视频）
        avformat_network_init();
        inInit = true;
    }

    dmux.unlock();
}


XDemux::~XDemux()
{

}


bool XDemux::open(const char *url)
{
    close();
    //参数设置
    AVDictionary *opts = NULL;
    //设置rtsp流已tcp协议打开
    av_dict_set(&opts, "rtsp_transport", "tcp", 0);

    //网络延时时间
    av_dict_set(&opts, "max_delay", "500", 0);

    mutex_.lock();
    int re = avformat_open_input(
        &ic,
        url,
        0,  // 0表示自动选择解封器
        &opts //参数设置，比如rtsp的延时时间
    );
    if (re != 0)
    {
        mutex_.unlock();
        char buf[1024] = { 0 };
        av_strerror(re, buf, sizeof(buf) - 1);
        cout << "open " << url << " failed! :" << buf << endl;
        return false;
    }
    cout << "open " << url << " success! " << endl;

    //获取流信息
    re = avformat_find_stream_info(ic, 0);

    //总时长 毫秒
    totalMs = ic->duration / (AV_TIME_BASE / 1000);
    cout << "totalMs = " << totalMs << endl;

    //打印视频流详细信息
    av_dump_format(ic, 0, url, 0);

    //获取视频流
    videoStream = av_find_best_stream(ic, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);

    AVStream *as = ic->streams[videoStream];
    width = as->codecpar->width;
    height = as->codecpar->height;

    cout << "codec_id = " << as->codecpar->codec_id << endl;
    cout << "format = " << as->codecpar->format << endl;
    cout << videoStream << " video info" << endl;
    cout << "width=" << as->codecpar->width << endl;
    cout << "height=" << as->codecpar->height << endl;
    //帧率 fps 分数转换
    cout<<"video fps = " <<r2d(as->avg_frame_rate)<<endl;

    //获取音频流
    audioStream = av_find_best_stream(ic, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    as = ic->streams[audioStream];
    sampleRate = as->codecpar->sample_rate;
    channels = as->codecpar->channels;

    cout << "codec_id = " << as->codecpar->codec_id << endl;
    cout << "format = " << as->codecpar->format << endl;
    cout << audioStream << " audio info" << endl;
    cout << "sample_rate = " << as->codecpar->sample_rate << endl;
    //AVSampleFormat;
    cout << "channels = " << as->codecpar->channels << endl;
    //一帧数据？？ 单通道样本数
    cout << "frame_size = " << as->codecpar->frame_size << endl;
    //1024 * 2 * 2 = 4096  fps = sample_rate/frame_size
    mutex_.unlock();

    return true;
}

// 空间需要调用者释放， 释放AVPacket对象空间和数据空间av_packet_free
AVPacket *XDemux::read()
{
    mutex_.lock();
    if(!ic)
    {
        mutex_.unlock();
        return nullptr;
    }
    AVPacket *pkt = av_packet_alloc();
    int re = av_read_frame(ic, pkt);
    if(re != 0)
    {
        mutex_.unlock();
        av_packet_free(&pkt);
        return nullptr;
    }
    // pts 转为毫秒
    pkt->pts = pkt->pts*(1000*r2d(ic->streams[pkt->stream_index]->time_base));
    pkt->dts = pkt->dts*(1000*r2d(ic->streams[pkt->stream_index]->time_base));
    mutex_.unlock();
    cout<<"pkt->pts = " << pkt->pts << flush;
    return pkt;
}

bool XDemux::isAudio(AVPacket *pkt)
{
    if (!pkt) return false;
    if (pkt->stream_index == videoStream)
        return false;
    return true;

}

// 获取视频参数 返回的空间需要清理 avcodec_parameters_free
AVCodecParameters *XDemux::copyVPara()
{
    mutex_.lock();
    if(!ic)
    {
        mutex_.unlock();
        return nullptr;
    }
    AVCodecParameters *pa = avcodec_parameters_alloc();
    avcodec_parameters_copy(pa, ic->streams[videoStream]->codecpar);
    mutex_.unlock();
    return pa;
}

// 获取音频参数 返回的空间需要清理 avcodec_parameters_free
AVCodecParameters *XDemux::copyAPara()
{
    mutex_.lock();
    if(!ic)
    {
        mutex_.unlock();
        return nullptr;
    }
    AVCodecParameters *pa = avcodec_parameters_alloc();
    avcodec_parameters_copy(pa, ic->streams[audioStream]->codecpar);
    mutex_.unlock();
    return pa;
}

// seek 位置pos从0.0-1.0
bool XDemux::seek(double pos)
{
    mutex_.lock();
    if (!ic)
    {
        mutex_.unlock();
        return false;
    }
    //清理读取缓冲
    avformat_flush(ic);

    long long seekPos = 0;
    seekPos = ic->streams[videoStream]->duration * pos;
    int re = av_seek_frame(ic, videoStream, seekPos, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME);
    mutex_.unlock();
    if (re < 0)
        return false;
    return true;
}

//清空读取缓存
void XDemux::clear()
{
    mutex_.lock();
    if (!ic)
    {
        mutex_.unlock();
        return ;
    }
    //清理读取缓冲
    avformat_flush(ic);
    mutex_.unlock();
}

void XDemux::close()
{
    mutex_.lock();
    if (!ic)
    {
        mutex_.unlock();
        return;
    }
    avformat_close_input(&ic);
    //媒体总时长（毫秒）
    totalMs = 0;
    mutex_.unlock();
}




