
#pragma once

#include <QOpenGLWidget>
#include <mutex>


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

class XVideoWidget : public QOpenGLWidget
{
	Q_OBJECT

public:
    void init(int width, int height);

	//不管成功与否都释放frame空间
    virtual void repaint(AVFrame *frame);

	XVideoWidget(QWidget *parent);
	~XVideoWidget();

private:
    int scaleImg(AVCodecContext *pCodecCtx,
                 AVFrame *src_picture,
                 AVFrame *dst_picture,
                 int nDstH,
                 int nDstW);

protected:
    void paintEvent(QPaintEvent *e);


private:
    std::mutex mutex_;
    SwsContext *swsCtxPtr_;
    AVPicture avPicture_;

    int width_ = 240;
    int height_ = 128;

};
