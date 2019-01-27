
#include "FFmpeg.h"



FFmpeg::FFmpeg(QObject *parent)
    : QObject(parent)
{
    m_codecCtxPtr = NULL;
    m_videoStream = -1;
    m_audioStream = -1;

}

FFmpeg::~FFmpeg()
{
    if(m_formatCtxPtr)
    {
        avformat_close_input(&m_formatCtxPtr);
        m_formatCtxPtr = nullptr;
    }
    sws_freeContext(m_swsCtxPtr);
}


static double r2d(AVRational r)
{
    return r.den == 0 ? 0:(double)r.num / (double)r.den;
}

int FFmpeg::initial(QString & url)
{
    int err;
    m_fileURL = url;
    AVCodec *pCodec;
    av_register_all(); // 初始化封装库
    avformat_network_init(); // 初始化网络库
    avcodec_register_all(); // 初始化解码器
    m_formatCtxPtr = avformat_alloc_context();
    m_framePtr = av_frame_alloc();
    err = avformat_open_input(&m_formatCtxPtr,
                              m_fileURL.toStdString().c_str(),
                              NULL, // 自动选择解封器
                              NULL); // 参数设置
    if (err < 0)
    {
        printf("Can not open this file");
        return -1;
    }
    if (avformat_find_stream_info(m_formatCtxPtr, NULL) < 0)
    {
        printf("Unable to get stream info");
        return -1;
    }

    // 总时长 毫秒
    int totalMs = m_formatCtxPtr->duration/(AV_TIME_BASE/1000);
    printf("total ms:%d\n", totalMs);

    // 打印视频流的详细信息
    av_dump_format(m_formatCtxPtr, 0, m_fileURL.toStdString().c_str(), 0);

    // 获取音视频流信息
    int i = 0;
    m_videoStream = -1;
    m_audioStream = -1;
    for (i = 0; i < m_formatCtxPtr->nb_streams; i++)
    {
        if (m_formatCtxPtr->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            printf("------------------------------------\n");
            printf("video info: %d.\n", i);
            printf("width: %d.\n", m_formatCtxPtr->streams[i]->codecpar->width);
            printf("height: %d.\n", m_formatCtxPtr->streams[i]->codecpar->height);
            printf("codec_id: %d.\n", m_formatCtxPtr->streams[i]->codecpar->codec_id);
            // 帧率 fps
            printf("avg_frame_rate: %6.2f.\n", r2d(m_formatCtxPtr->streams[i]->avg_frame_rate));
            printf("------------------------------------\n");
            //m_audioStream = i;
            m_videoStream = i;
            //break;
        }
        else if (m_formatCtxPtr->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            printf("------------------------------------\n");
            printf("audio info: %d.\n", i);
            printf("sample_rate: %d.\n", m_formatCtxPtr->streams[i]->codecpar->sample_rate);
            printf("format: %d.\n", m_formatCtxPtr->streams[i]->codecpar->format);
            printf("channels: %d.\n", m_formatCtxPtr->streams[i]->codecpar->channels);
            printf("codec_id: %d.\n", m_formatCtxPtr->streams[i]->codecpar->codec_id);
            // 帧率 fps
            printf("avg_frame_rate: %6.2f.\n", r2d(m_formatCtxPtr->streams[i]->avg_frame_rate));
            // 一帧数据 单通道一定数量样本数
            printf("frame_size: %d.\n", m_formatCtxPtr->streams[i]->codecpar->frame_size);
            // fps = sample_rate/frame_size
            printf("------------------------------------\n");
            m_audioStream = i;
            //break;
        }
    }

    // 方式2
//    m_videoStream = av_find_best_stream(m_formatCtxPtr, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);

    if (m_videoStream == -1)
    {
        printf("Unable to find video stream.");
        return -1;
    }
    /*
    if (m_audioStream == -1)
    {
        printf("Unable to find audio stream.");
        return -1;
    }
    */
    m_codecCtxPtr = m_formatCtxPtr->streams[m_videoStream]->codec;

    m_width = m_codecCtxPtr->width;
    m_height = m_codecCtxPtr->height;
    avpicture_alloc(&m_picture, AV_PIX_FMT_RGB24, m_codecCtxPtr->width, m_codecCtxPtr->height);
    pCodec = avcodec_find_decoder(m_codecCtxPtr->codec_id);
    m_swsCtxPtr = sws_getContext(m_width, m_height, AV_PIX_FMT_YUV420P, m_width,
            m_height, AV_PIX_FMT_RGB24,
            SWS_BICUBIC, 0, 0, 0);

    if (pCodec == NULL)
    {
        printf("Unsupported codec");
        return -1;
    }
    printf("video size : width=%d height=%d \n", m_codecCtxPtr->width, m_codecCtxPtr->height);
    if (avcodec_open2(m_codecCtxPtr, pCodec, NULL) < 0)
    {
        printf("Unable to open codec");
        return -1;
    }
    printf("initial successfully");
    return 0;
}

int FFmpeg::h264Decodec(QWidget *player)
{
    int frameFinished = 0;
    while (av_read_frame(m_formatCtxPtr, &m_packet) >= 0)
    {
        printf("------------------------------------\n");
        printf("m_packet.size: %d.\n", m_packet.size);
        // 显示时间
        printf("m_packet.pts: %lld.\n", m_packet.pts);
        printf("m_packet.pts ms: %lld.\n", m_packet.pts * (r2d(m_formatCtxPtr->streams[m_packet.stream_index]->avg_frame_rate) * 1000));
        // 解码时间
        printf("m_packet.dts: %lld.\n", m_packet.dts);

        if(m_packet.stream_index == m_videoStream)
        {
            printf("Video Stream\n");
            avcodec_decode_video2(m_codecCtxPtr, m_framePtr, &frameFinished, &m_packet);
            if (frameFinished)
            {
                printf("***************ffmpeg decodec*******************\n");
                m_mutex.lock();
                int rs = sws_scale(m_swsCtxPtr, (const uint8_t* const *) m_framePtr->data,
                                   m_framePtr->linesize, 0,
                                   m_height, m_picture.data, m_picture.linesize);
                player->update();
                m_mutex.unlock();
                if (rs == -1)
                {
                    printf("__________Can open to change to des imag_____________e\n");
                    return -1;
                }
            }
        }
        else if(m_packet.stream_index == m_audioStream)
        {
            printf("Audio Stream\n");
        }
    }
    return 1;

}
