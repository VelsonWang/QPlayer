
#ifndef XSDLVIDEOWIDGET_H
#define XSDLVIDEOWIDGET_H

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
#include "SDL.h"

#ifdef __cplusplus
}
#endif

class XSDLVideoWidget : public QWidget, public IVideoCall
{
	Q_OBJECT

public:
    XSDLVideoWidget(QWidget *parent);
    ~XSDLVideoWidget();

    virtual void init(int width, int height);
    virtual void repaint(AVFrame *frame);

private:    
    SDL_Window *screen_;
    SDL_Renderer* sdlRenderer_;
    SDL_Texture* sdlTexture_;

    std::mutex mutex_;
    SwsContext *swsCtxPtr_;
    AVPicture avPicture_;

    int width_ = 800;
    int height_ = 480;

};

#endif


