#ifndef PLAYER_H
#define PLAYER_H

#include <QWidget>
#include <QThread>
#include <QPaintEvent>
#include "FFmpeg.h"

class Player;

class PlayThread : public QThread
{
public :
    PlayThread();
    ~PlayThread();
    void run();
    void setURL(QString url);
    void setPlayer(Player * player);

private:
    QString m_url;
    Player *m_playerPtr;
};

/////////////////////////////////////////////////

class Player : public QWidget
{
    Q_OBJECT
public:
    explicit Player(QWidget *parent = nullptr);
    ~Player();

public:
    void setFFmpeg(FFmpeg * ffmpeg);
    FFmpeg * getFFmpeg();

protected:
    void paintEvent(QPaintEvent *event);

signals:

public slots:


private:
    FFmpeg * m_ffmpegPtr;
};




#endif // PLAYER_H
