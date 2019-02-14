
#ifndef IVIDEOCALL_H
#define IVIDEOCALL_H

struct AVFrame;

class IVideoCall
{
public:
    virtual void init(int width, int height) = 0;
    virtual void repaint(AVFrame *frame) = 0;
};

#endif

