#ifndef XDECOCE_H
#define XDECOCE_H

struct AVCodecParameters;
struct AVCodecContext;
struct AVFrame;
struct AVPacket;

#include <mutex>

extern void XFreePacket(AVPacket **pkt);
extern void XFreeFrame(AVFrame **frame);

class XDecode
{
public:
    XDecode();
    virtual ~XDecode();

	//打开解码器,不管成功与否都释放para空间
    virtual bool open(AVCodecParameters *para);

    //发送到解码线程，不管成功与否都释放pkt空间（对象和媒体内容）
    virtual bool send(AVPacket *pkt);

    //获取解码数据，一次send可能需要多次Recv，获取缓冲中的数据Send NULL在Recv多次
    //每次复制一份，由调用者释放 av_frame_free
    virtual AVFrame* recv();

    virtual void close();
    virtual void clear();

public:
    bool isAudio_ = false;
    //当前解码到的pts
    long long pts_ = 0;

protected:
    AVCodecContext *codec_ = 0;
    std::mutex mutex_;
};

#endif
