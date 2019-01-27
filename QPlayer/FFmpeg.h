
#ifndef FFMPEG_H_
#define FFMPEG_H_

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

#include <QWidget>
#include <QReadWriteLock>
#include <QMutex>
#include <QObject>
#include <QWaitCondition>
#include <QString>

class FFmpeg : public QObject
{
    Q_OBJECT
public:
    FFmpeg(QObject *parent = nullptr);
    virtual ~FFmpeg();

    int initial(QString & url);
    int h264Decodec(QWidget *player);

    friend class Player;

private:
    AVFormatContext *m_formatCtxPtr;
    AVCodecContext *m_codecCtxPtr;
    AVFrame *m_framePtr;
    AVPacket m_packet;
    AVPicture  m_picture;
    SwsContext * m_swsCtxPtr;
    int m_videoStream;
    int m_audioStream;
    int m_width;
    int m_height;
    QMutex m_mutex;
    QString m_fileURL;

};

#endif /* FFMPEG_H_ */
