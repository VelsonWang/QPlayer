#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "FFmpeg.h"
#include "Player.h"
#include <QApplication>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_playerPtr(nullptr)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("QPlayer"));
    m_playerPtr = new Player(this);
    this->setCentralWidget(m_playerPtr);

    //on_actionOpen_triggered();
}

MainWindow::~MainWindow()
{
    delete m_playerPtr;
    delete ui;
}

/*
* 打开
*/
void MainWindow::on_actionOpen_triggered()
{
    //ui->rtspURL->setText("rtsp://184.72.239.149/vod/mp4://BigBuckBunny_175k.mov");

    qDebug()<<"on_actionOpen_triggered()";

    FFmpeg * ffmpeg = new FFmpeg();
    m_playerPtr->setFFmpeg(ffmpeg);
    PlayThread *threadPtr = new PlayThread();

    //threadPtr->setURL("rtsp://184.72.239.149/vod/mp4://BigBuckBunny_175k.mov");
    threadPtr->setURL(QApplication::applicationDirPath() + "/test.mp4");
    threadPtr->setPlayer(m_playerPtr);
    threadPtr->start();
    m_playerPtr->show();
}
