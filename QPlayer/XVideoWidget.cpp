
#include "XVideoWidget.h"
#include <QPainter>
#include <QTimer>
#include <QDebug>

//自动加双引号
#define GET_STR(x) #x


XVideoWidget::XVideoWidget(QWidget *parent)
    : QOpenGLWidget(parent),
      bShowVideo_(false) {

}

XVideoWidget::~XVideoWidget() {
    avpicture_free(&avPicture_);
    sws_freeContext(swsCtxPtr_);
}


int XVideoWidget::scaleImg(AVCodecContext *pCodecCtx,
                           AVFrame *src_picture,
                           AVFrame *dst_picture,
                           int nDstH,
                           int nDstW) {
    int i = 0;
    int nSrcStride[3];
    int nDstStride[3];
    int nSrcH = pCodecCtx->height;
    int nSrcW = pCodecCtx->width;
    struct SwsContext* m_pSwsContext;

    uint8_t *pSrcBuff[3] = {src_picture->data[0], src_picture->data[1], src_picture->data[2]};

    nSrcStride[0] = nSrcW ;
    nSrcStride[1] = nSrcW/2 ;
    nSrcStride[2] = nSrcW/2 ;

    dst_picture->linesize[0] = nDstW;
    dst_picture->linesize[1] = nDstW/2;
    dst_picture->linesize[2] = nDstW/2;

    m_pSwsContext = sws_getContext(nSrcW,
                                   nSrcH,
                                   AV_PIX_FMT_YUV420P,
                                   nDstW,
                                   nDstH,
                                   AV_PIX_FMT_YUV420P,
                                   SWS_BICUBIC,
                                   NULL,
                                   NULL,
                                   NULL);

    if (NULL == m_pSwsContext) {
        return 0;
    }

    sws_scale(m_pSwsContext,
              src_picture->data,
              src_picture->linesize,
              0,
              pCodecCtx->height,
              dst_picture->data,
              dst_picture->linesize);

    sws_freeContext(m_pSwsContext);

    return 1;
}


void XVideoWidget::repaint(AVFrame *frame) {
    if (!frame)
        return;
    std::unique_lock<std::mutex> lock(mutex_);

    if (width_*height_ == 0 || frame->width != width_ || frame->height != height_) {
		av_frame_free(&frame);
        return;
	}

    int rs = sws_scale(swsCtxPtr_,
                       (const uint8_t* const *) frame->data,
                       frame->linesize,
                       0,
                       height_,
                       avPicture_.data,
                       avPicture_.linesize);

    bShowVideo_ = true;
	update();
}


void XVideoWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    if(avPicture_.data != NULL && bShowVideo_) {
        QImage image = QImage(avPicture_.data[0], width_, height_, QImage::Format_RGB888);
        QPixmap pix = QPixmap::fromImage(image);
        painter.drawPixmap((this->width()-pix.width())/2, (this->height()-pix.height())/2, pix.width(), pix.height(), pix);
    }
    painter.end();
}

void XVideoWidget::init(int width, int height) {
    std::unique_lock<std::mutex> lock(mutex_);
    width_ = width;
    height_ = height;

    avpicture_alloc(&avPicture_, AV_PIX_FMT_RGB24, width_, height_);
    swsCtxPtr_ = sws_getContext(width_,
                                height_,
                                AV_PIX_FMT_YUV420P,
                                width_,
                                height_,
                                AV_PIX_FMT_RGB24,
                                SWS_BICUBIC,
                                0,
                                0,
                                0);
}




