﻿#ifndef XDEMUX_H
#define XDEMUX_H

#include <mutex>

struct AVFormatContext;
struct AVPacket;
struct AVCodecParameters;

class XDemux
{
public:
    // 打开媒体文件或流媒体 rtmp http rtsp
    XDemux();
    virtual ~XDemux();

public:
    virtual bool open(const char *url);
    virtual AVPacket *read();
    //只读视频，音频丢弃空间释放
    virtual AVPacket *readVideo();
    virtual bool isAudio(AVPacket *pkt);
    // 获取视频参数
    AVCodecParameters *copyVPara();
    // 获取音频参数
    AVCodecParameters *copyAPara();
    // seek 位置pos从0.0-1.0
    virtual bool seek(double pos);
    //清空读取缓存
    virtual void clear();
    virtual void close();

public:
    //媒体总时长(毫秒)
    int totalMs_;
    int width_ = 0;
    int height_ = 0;
    int sampleRate_ = 0;
    int channels_ = 0;

protected:
    std::mutex mutex_;
    //解封装上下文
    AVFormatContext *ic_ = NULL;
    //音视频索引，读取时区分音视频
    int videoStream_ = 0;
    int audioStream_ = 1;
};

#endif // XDEMUX_H
