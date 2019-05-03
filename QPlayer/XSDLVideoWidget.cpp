
#include "XSDLVideoWidget.h"
#include <QPainter>
#include <QTimer>
#include <QDebug>

#if 0
    程序使用SDL2播放RGB/YUV视频像素数据。SDL实际上是对底层绘图
    API（Direct3D，OpenGL）的封装，使用起来明显简单于直接调用底层
    API。

    函数调用步骤如下:

    [初始化]
    SDL_Init(): 初始化SDL。
    SDL_CreateWindow(): 创建窗口（Window）。
    SDL_CreateRenderer(): 基于窗口创建渲染器（Render）。
    SDL_CreateTexture(): 创建纹理（Texture）。

    [循环渲染数据]
    SDL_UpdateTexture(): 设置纹理的数据。
    SDL_RenderCopy(): 纹理复制给渲染器。
    SDL_RenderPresent(): 显示。

#endif

XSDLVideoWidget::XSDLVideoWidget(QWidget *parent)
    : QWidget(parent) {
    if(SDL_Init(SDL_INIT_VIDEO)) {
        qDebug( "Could not initialize SDL - %s\n", SDL_GetError());
        return;
    }
#if 1
    screen_ = SDL_CreateWindowFrom((void *)(this->winId()));
#else
    //SDL 2.0 Support for multiple windows
    screen_ = SDL_CreateWindow("QPlay SDL2",
                               SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED,
                               width_,
                               height_,
                               SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
#endif
    if(!screen_) {
        qDebug("SDL: could not create window - exiting:%s\n", SDL_GetError());
        return;
    }

    sdlRenderer_ = SDL_CreateRenderer(screen_, -1, 0);

    Uint32 pixformat=0;

    //IYUV: Y + U + V  (3 planes)
    //YV12: Y + V + U  (3 planes)
    pixformat = SDL_PIXELFORMAT_IYUV;

    sdlTexture_ = SDL_CreateTexture(sdlRenderer_,
                                    pixformat,
                                    SDL_TEXTUREACCESS_STREAMING,
                                    width_,
                                    height_);

}

XSDLVideoWidget::~XSDLVideoWidget() {
    avpicture_free(&avPicture_);
    sws_freeContext(swsCtxPtr_);
    SDL_Quit();
}


void XSDLVideoWidget::repaint(AVFrame *frame)
{
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

    SDL_Rect sdlRect;

    SDL_UpdateTexture(sdlTexture_, NULL, avPicture_.data[0], width_);

    //FIX: If window is resize
    sdlRect.x = 0;
    sdlRect.y = 0;
    sdlRect.w = width_;
    sdlRect.h = height_;

    SDL_RenderClear(sdlRenderer_);
    SDL_RenderCopy(sdlRenderer_, sdlTexture_, NULL, &sdlRect);
    SDL_RenderPresent(sdlRenderer_);
}

void XSDLVideoWidget::init(int width, int height) {
    std::unique_lock<std::mutex> lock(mutex_);
    width_ = width;
    height_ = height;

    avpicture_alloc(&avPicture_, AV_PIX_FMT_YUV420P, width_, height_);
    swsCtxPtr_ = sws_getContext(width_,
                                height_,
                                AV_PIX_FMT_YUV420P,
                                width_,
                                height_,
                                AV_PIX_FMT_YUV420P,
                                SWS_BICUBIC,
                                0,
                                0,
                                0);
}




