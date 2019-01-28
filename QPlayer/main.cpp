#include "MainWindow.h"
#include <QApplication>
#include <QTextCodec>

#ifdef __cplusplus
extern "C"{
#endif

#include <SDL.h>
#include <SDL_thread.h>

#ifdef __cplusplus
}
#endif

///////////////// for test >>>

#include <iostream>
#include <QThread>
#include "FFmpeg.h"
#include "Player.h"
#include "XDemux.h"
#include "XDecode.h"
#include "XPlay2.h"
#include "XResample.h"
#include "XAudioPlay.h"

///////////////// for test  <<<


#ifdef __MINGW32__
#undef main /* Prevents SDL from overriding main() */
#endif



class TestThread :public QThread
{
public:
    void Init()
    {
        char *url = "test.mp4";
        std::cout << "demux.Open = " << demux.open(url);
        std::cout << "CopyVPara = " << demux.copyVPara() << std::endl;
        std::cout << "CopyAPara = " << demux.copyAPara() << std::endl;
        //cout << "seek=" << demux.Seek(0.95) << endl;

        /////////////////////////////

        std::cout << "vdecode.Open() = " << vdecode.open(demux.copyVPara()) << std::endl;
        //vdecode.Clear();
        //vdecode.Close();
        std::cout << "adecode.Open() = " << adecode.open(demux.copyAPara()) << std::endl;
        std::cout << "resample.Open = " << resample.open(demux.copyAPara()) << std::endl;

        XAudioPlay::get()->channels = demux.channels;
        XAudioPlay::get()->sampleRate = demux.sampleRate;
        std::cout << "XAudioPlay::get()->open() = " << XAudioPlay::get()->open()<< std::endl;

    }
    unsigned char *pcm = new unsigned char[1024 * 1024];
    void run()
    {
        for (;;)
        {
            AVPacket *pkt = demux.read();
            if (demux.isAudio(pkt))
            {
                adecode.send(pkt);
                AVFrame *frame = adecode.recv();
                int len = resample.resample(frame, pcm);
                std::cout<<"Resample:"<<len<<" ";
                while (len > 0)
                {
                    if (XAudioPlay::get()->getFree() >= len)
                    {
                        XAudioPlay::get()->write(pcm, len);
                        break;
                    }
                    msleep(1);
                }
            }
            else
            {
                vdecode.send(pkt);
                AVFrame *frame = vdecode.recv();
                video->repaint(frame);
                av_frame_free(&frame);
                msleep(40);
                //cout << "Video:" << frame << endl;
            }
            if (!pkt)break;
        }
    }
    ///测试XDemux
    XDemux demux;
    ///解码测试
    XDecode vdecode;
    XDecode adecode;
    XVideoWidget *video;
    XResample resample;
};

int main(int argc, char *argv[])
{
//    QApplication a(argc, argv);
//    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
//    QTextCodec::setCodecForLocale(codec);


    //const char *url = "rtmp://live.hkstv.hk.lxdns.com/live/hks";
    //const char *url = "rtsp://184.72.239.149/vod/mp4://BigBuckBunny_175k.mov";
//    const char *url = "test.mp4";

/*
    for(;;)
    {
        AVPacket *pkt = demux.read();
        if (demux.isAudio(pkt))
        {
            adecode.send(pkt);
            AVFrame *frame = adecode.recv();
            //cout << "Audio:" << frame << endl;
        }
        else
        {
            vdecode.send(pkt);
            AVFrame *frame = vdecode.recv();
            //cout << "Video:" << frame << endl;
        }

        if(pkt == nullptr) break;

    }


    MainWindow w;
    w.show();
*/


    QApplication a(argc, argv);
    XPlay2 w;
    w.show();

    TestThread tt;
    tt.Init();

    //初始化gl窗口
    w.ui.video->init(tt.demux.width, tt.demux.height);
    tt.video = w.ui.video;
    tt.start();

    int ret = a.exec();

    tt.terminate();
    tt.wait();

    return ret;
}
