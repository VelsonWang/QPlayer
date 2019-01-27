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
#include "FFmpeg.h"
#include "Player.h"
#include "XDemux.h"
#include "XDecode.h"

///////////////// for test  <<<


#ifdef __MINGW32__
#undef main /* Prevents SDL from overriding main() */
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);


    //const char *url = "rtmp://live.hkstv.hk.lxdns.com/live/hks";
    //const char *url = "rtsp://184.72.239.149/vod/mp4://BigBuckBunny_175k.mov";
    const char *url = "test.mp4";
    XDemux demux;
    demux.open(url);
    std::cout << demux.copyVPara() << std::endl;
    std::cout << demux.copyAPara() << std::endl;


    XDecode vdecode;
    std::cout << "vdecode.Open() = " << vdecode.open(demux.copyVPara()) << std::endl;
    vdecode.clear();
    vdecode.close();
    XDecode adecode;
    std::cout << "adecode.Open() = " << adecode.open(demux.copyAPara()) << std::endl;


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

    return a.exec();
}
