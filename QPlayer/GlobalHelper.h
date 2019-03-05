#ifndef GLOBALHELPER_H
#define GLOBALHELPER_H

enum ERROR_CODE
{
    NoError = 0,
    ErrorFileInvalid
};


#include <QString>
#include <QPushButton>
#include <QDebug>
#include <QStringList>

class GlobalHelper
{
public:
    GlobalHelper();

    static QString getQssStr(QString strQssPath);
    static void setIcon(QPushButton* btn, int iconSize, QChar icon);

    static void savePlaylist(QStringList& playList);
    static void getPlaylist(QStringList& playList);
    static void savePlayVolume(double& nVolume);
    static void getPlayVolume(double& nVolume);

    static QString getAppVersion();
};

//必须加以下内容,否则编译不能通过,为了兼容C和C99标准
#ifndef INT64_C
#define INT64_C
#define UINT64_C
#endif

extern "C"{
#include "libavutil/avstring.h"
#include "libavutil/eval.h"
#include "libavutil/mathematics.h"
#include "libavutil/pixdesc.h"
#include "libavutil/imgutils.h"
#include "libavutil/dict.h"
#include "libavutil/parseutils.h"
#include "libavutil/samplefmt.h"
#include "libavutil/avassert.h"
#include "libavutil/time.h"
#include "libavformat/avformat.h"
#include "libavdevice/avdevice.h"
#include "libswscale/swscale.h"
#include "libavutil/opt.h"
#include "libavcodec/avfft.h"
#include "libswresample/swresample.h"
}

#include "SDL.h"


#define MAX_SLIDER_VALUE 65536



#endif // GLOBALHELPER_H
