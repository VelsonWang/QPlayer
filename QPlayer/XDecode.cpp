
#include "XDecode.h"

extern "C"
{
#include<libavcodec/avcodec.h>
}

#include <iostream>

using namespace std;


void XFreePacket(AVPacket **pkt) {
    if (!pkt || !(*pkt)) return;
    av_packet_free(pkt);
}

void XFreeFrame(AVFrame **frame) {
    if (!frame || !(*frame)) return;
    av_frame_free(frame);
}


XDecode::XDecode() {
}


XDecode::~XDecode() {
}

//打开解码器
bool XDecode::open(AVCodecParameters *para) {
	if (!para) return false;
    close();
	//////////////////////////////////////////////////////////
	///解码器打开
	///找到解码器
	AVCodec *vcodec = avcodec_find_decoder(para->codec_id);
    if (!vcodec) {
		avcodec_parameters_free(&para);
        return false;
	}

    std::unique_lock<std::mutex> lock(mutex_);
    codec_ = avcodec_alloc_context3(vcodec);

	///配置解码器上下文参数
    avcodec_parameters_to_context(codec_, para);
	avcodec_parameters_free(&para);

	//八线程解码
    codec_->thread_count = 8;

	///打开解码器上下文
    int ret = avcodec_open2(codec_, 0, 0);
    if (ret != 0) {
        avcodec_free_context(&codec_);
        char buf[1024] = { 0 };
        av_strerror(ret, buf, sizeof(buf) - 1);
        return false;
	}
    return true;
}

//发送到解码线程，不管成功与否都释放pkt空间（对象和媒体内容）
bool XDecode::send(AVPacket *pkt) {
    //容错处理
    if (!pkt || pkt->size <= 0 || !pkt->data)
        return false;
    std::unique_lock<std::mutex> lock(mutex_);
    if (!codec_) {
        return false;
    }
    int ret = avcodec_send_packet(codec_, pkt);
    av_packet_free(&pkt);
    return ret == 0;
}

//获取解码数据，一次send可能需要多次Recv，获取缓冲中的数据Send NULL在Recv多次
//每次复制一份，由调用者释放 av_frame_free
AVFrame* XDecode::recv() {
    std::unique_lock<std::mutex> lock(mutex_);
    if (!codec_) {
        return NULL;
    }
    AVFrame *frame = av_frame_alloc();
    int ret = avcodec_receive_frame(codec_, frame);
    if (ret != 0) {
        av_frame_free(&frame);
        return NULL;
    }
    pts_ = frame->pts;
    return frame;
}

void XDecode::close() {
    std::unique_lock<std::mutex> lock(mutex_);
    if (codec_) {
        avcodec_close(codec_);
        avcodec_free_context(&codec_);
    }
    pts_ = 0;
}

void XDecode::clear() {
    std::unique_lock<std::mutex> lock(mutex_);
    //清理解码缓冲
    if (codec_)
        avcodec_flush_buffers(codec_);
}



