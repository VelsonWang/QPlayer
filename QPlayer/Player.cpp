#include <QPainter>
#include <QDebug>
#include "Player.h"

PlayThread::PlayThread()
    : m_playerPtr(nullptr)
{

}

PlayThread::~PlayThread()
{

}

void PlayThread::run()
{
    if(m_playerPtr != nullptr)
    {
        m_playerPtr->getFFmpeg()->initial(m_url);
        m_playerPtr->getFFmpeg()->h264Decodec(m_playerPtr);
    }
}


void PlayThread::setURL(QString url)
{
    m_url = url;
}
void PlayThread::setPlayer(Player * player)
{
    m_playerPtr = player;
}


/////////////////////////////////////////////////////////

Player::Player(QWidget *parent) :
    QWidget(parent),
    m_ffmpegPtr(nullptr)
{

}

Player::~Player()
{
    if(m_ffmpegPtr != nullptr)
        delete m_ffmpegPtr;
}


void Player::setFFmpeg(FFmpeg * ffmpeg)
{
    m_ffmpegPtr = ffmpeg;
}

FFmpeg * Player::getFFmpeg()
{
    return m_ffmpegPtr;
}

void Player::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    if(m_ffmpegPtr == nullptr)
    {
        painter.fillRect(rect(), Qt::black);
        return;
    }

    if(m_ffmpegPtr->m_picture.data != NULL)
    {
        //if(m_ffmpegPtr->m_mutex.tryLock(1000))
        {
            //m_ffmpegPtr->m_mutex.lock();
            QImage image = QImage(m_ffmpegPtr->m_picture.data[0], m_ffmpegPtr->m_width, m_ffmpegPtr->m_height, QImage::Format_RGB888);
            QPixmap pix = QPixmap::fromImage(image);
            painter.drawPixmap(0, 0, pix.width(), pix.height(), pix);
            if(this->size().width() != pix.width() || this->size().height() != pix.height())
                this->resize(pix.width(), pix.height());
            update();
            //m_ffmpegPtr->m_mutex.unlock();
        }
    }
}


