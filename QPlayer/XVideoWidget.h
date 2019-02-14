
#ifndef XVIDEOWIDGET_H
#define XVIDEOWIDGET_H

#include <QOpenGLWidget>
#include <QResizeEvent>
#include <mutex>
#include "IVideoCall.h"

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

class XVideoWidget : public QOpenGLWidget, public IVideoCall
{
	Q_OBJECT

public:
    XVideoWidget(QWidget *parent);
    ~XVideoWidget();

    virtual void init(int width, int height);
    virtual void repaint(AVFrame *frame);

private:
    int scaleImg(AVCodecContext *pCodecCtx,
                 AVFrame *src_picture,
                 AVFrame *dst_picture,
                 int nDstH,
                 int nDstW);

protected:
    void paintEvent(QPaintEvent *event);

private:
    std::mutex mutex_;
    SwsContext *swsCtxPtr_;
    AVPicture avPicture_;
    bool bShowVideo_;

    int width_ = 240;
    int height_ = 128;

};

#endif


