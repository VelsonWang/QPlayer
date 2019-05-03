#include <QFile>
#include <QPainter>
#include <QtMath>
#include <QDebug>
#include <QAbstractItemView>
#include <QMimeData>
#include <QSizeGrip>
#include <QWindow>
#include <QDesktopWidget>
#include <QScreen>
#include <QRect>
#include <QFileDialog>
#include <QMessageBox>

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "GlobalHelper.h"
#include "XDemuxThread.h"

const int FULLSCREEN_MOUSE_DETECT_TIME = 200;

static XDemuxThread demuxThread;


MainWindow::MainWindow(QMainWindow *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      m_nShadowWidth(0),
      m_stMenu(this),
      m_stPlaylist(this),
      m_stTitle(this),
      m_bMoveDrag(false) {
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint /*| Qt::WindowSystemMenuHint*/ | Qt::WindowMinimizeButtonHint);
    this->setWindowIcon(QIcon(":/Resources/player.png"));

    //加载样式
    QString qss = GlobalHelper::getQssStr(":/Resources/qss/mainwid.css");
    setStyleSheet(qss);

    // 追踪鼠标 用于播放时隐藏鼠标
    this->setMouseTracking(true);

    //ui->ShowWid->setMouseTracking(true);

    //保证窗口不被绘制上的部分透明
    //setAttribute(Qt::WA_TranslucentBackground);

    //接受放下事件
    //setAcceptDrops(true);
    //可以清晰地看到放下过程中的图标指示
    //setDropIndicatorShown(true);

//    setAcceptDrops(true);
//    setDragDropMode(QAbstractItemView::DragDrop);
//    setDragEnabled(true);
//    setDropIndicatorShown(true);

    //窗口大小调节
//    QSizeGrip   *pSizeGrip = new QSizeGrip(this);
//    pSizeGrip->setMinimumSize(10, 10);
//    pSizeGrip->setMaximumSize(10, 10);
//    ui->verticalLayout->addWidget(pSizeGrip, 0, Qt::AlignBottom | Qt::AlignRight);

    m_bPlaying = false;

    m_bFullScreenPlay = false;

    m_stCtrlBarAnimationTimer.setInterval(1000);
    m_stFullscreenMouseDetectTimer.setInterval(FULLSCREEN_MOUSE_DETECT_TIME);

    demuxThread.start();
    startTimer(40);
}

MainWindow::~MainWindow()
{
    demuxThread.close();
    delete ui;
}

bool MainWindow::init()
{
    QWidget *em = new QWidget(this);
    ui->PlaylistWid->setTitleBarWidget(em);
    ui->PlaylistWid->setWidget(&m_stPlaylist);
    //ui->PlaylistWid->setFixedWidth(100);

    QWidget *emTitle = new QWidget(this);
    ui->TitleWid->setTitleBarWidget(emTitle);
    ui->TitleWid->setWidget(&m_stTitle);
    
    //连接自定义信号与槽
    if (connectSignalSlots() == false) {
        return false;
    }

    if (ui->CtrlBarWid->init() == false ||
        m_stPlaylist.init() == false ||
        m_stTitle.init() == false) {
        return false;
    }

    m_stCtrlbarAnimationShow = new QPropertyAnimation(ui->CtrlBarWid, "geometry");
    m_stCtrlbarAnimationHide = new QPropertyAnimation(ui->CtrlBarWid, "geometry");

    if (m_stAboutWidget.init() == false) {
        return false;
    }

    m_stActFullscreen.setText(tr("全屏"));
    m_stActFullscreen.setCheckable(true);
    m_stMenu.addAction(&m_stActFullscreen);

    m_stActOpen.setText(tr("打开"));
    m_stMenu.addAction(&m_stActOpen);

    m_stActAbout.setText(tr("关于"));
    m_stMenu.addAction(&m_stActAbout);
    
    m_stActExit.setText(tr("退出"));
    m_stMenu.addAction(&m_stActExit);

    return true;
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
}


void MainWindow::enterEvent(QEvent *event)
{
    Q_UNUSED(event);

}

void MainWindow::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);

}

bool MainWindow::connectSignalSlots()
{
    //连接信号与槽
    connect(&m_stTitle, &Title::sigCloseBtnClicked, this, &MainWindow::onCloseBtnClicked);
    connect(&m_stTitle, &Title::sigMaxBtnClicked, this, &MainWindow::onMaxBtnClicked);
    connect(&m_stTitle, &Title::sigMinBtnClicked, this, &MainWindow::onMinBtnClicked);
    connect(&m_stTitle, &Title::sigDoubleClicked, this, &MainWindow::onMaxBtnClicked);
    connect(&m_stTitle, &Title::sigFullScreenBtnClicked, this, &MainWindow::onFullScreenPlay);
    connect(&m_stTitle, &Title::sigOpenFile, &m_stPlaylist, &PlayList::onAddFileAndPlay);
    connect(&m_stTitle, &Title::sigShowMenu, this, &MainWindow::onShowMenu);
    

//    connect(&m_stPlaylist, &Playlist::SigPlay, ui->ShowWid, &Show::SigPlay);

//    connect(ui->VideoShow, &Show::sigOpenFile, &m_stPlaylist, &Playlist::OnAddFileAndPlay);
//    connect(ui->ShowWid, &Show::SigFullScreen, this, &MainWindow::OnFullScreenPlay);
//    connect(ui->ShowWid, &Show::SigPlayOrPause, VideoCtl::GetInstance(), &VideoCtl::OnPause);
//    connect(ui->ShowWid, &Show::SigStop, VideoCtl::GetInstance(), &VideoCtl::OnStop);
//    connect(ui->ShowWid, &Show::SigShowMenu, this, &MainWindow::OnShowMenu);

    connect(ui->CtrlBarWid, &CtrlBar::sigShowOrHidePlaylist, this, &MainWindow::onShowOrHidePlaylist);
    connect(ui->CtrlBarWid, &CtrlBar::sigPlaySeek, this, &MainWindow::onPlaySeek);
//    connect(ui->CtrlBarWid, &CtrlBar::SigPlayVolume, VideoCtl::GetInstance(), &VideoCtl::OnPlayVolume);
    connect(ui->CtrlBarWid, &CtrlBar::sigPlayOrPause, this, &MainWindow::onPlayOrPause);
//    connect(ui->CtrlBarWid, &CtrlBar::SigStop, VideoCtl::GetInstance(), &VideoCtl::OnStop);
//    connect(ui->CtrlBarWid, &CtrlBar::SigBackwardPlay, &m_stPlaylist, &Playlist::OnBackwardPlay);
//    connect(ui->CtrlBarWid, &CtrlBar::SigForwardPlay, &m_stPlaylist, &Playlist::OnForwardPlay);
    connect(ui->CtrlBarWid, &CtrlBar::sigShowMenu, this, &MainWindow::onShowMenu);
//    connect(ui->CtrlBarWid, &CtrlBar::SigShowSetting, this, &MainWindow::OnShowSettingWid);

    connect(this, &MainWindow::sigShowMax, &m_stTitle, &Title::onChangeMaxBtnStyle);
//    connect(this, &MainWindow::SigSeekForward, VideoCtl::GetInstance(), &VideoCtl::OnSeekForward);
//    connect(this, &MainWindow::SigSeekBack, VideoCtl::GetInstance(), &VideoCtl::OnSeekBack);
//    connect(this, &MainWindow::SigAddVolume, VideoCtl::GetInstance(), &VideoCtl::OnAddVolume);
//    connect(this, &MainWindow::SigSubVolume, VideoCtl::GetInstance(), &VideoCtl::OnSubVolume);
    connect(this, &MainWindow::sigOpenFile, &m_stPlaylist, &PlayList::onAddFileAndPlay);
    
    
//    connect(VideoCtl::GetInstance(), &VideoCtl::SigVideoTotalSeconds, ui->CtrlBarWid, &CtrlBar::OnVideoTotalSeconds);
//    connect(VideoCtl::GetInstance(), &VideoCtl::SigVideoPlaySeconds, ui->CtrlBarWid, &CtrlBar::OnVideoPlaySeconds);
//    connect(VideoCtl::GetInstance(), &VideoCtl::SigVideoVolume, ui->CtrlBarWid, &CtrlBar::OnVideopVolume);
//    connect(VideoCtl::GetInstance(), &VideoCtl::SigPauseStat, ui->CtrlBarWid, &CtrlBar::OnPauseStat);
//    connect(VideoCtl::GetInstance(), &VideoCtl::SigStopFinished, ui->CtrlBarWid, &CtrlBar::OnStopFinished, Qt::DirectConnection);
//    connect(VideoCtl::GetInstance(), &VideoCtl::SigStopFinished, ui->ShowWid, &Show::OnStopFinished, Qt::DirectConnection);
//    connect(VideoCtl::GetInstance(), &VideoCtl::SigStopFinished, &m_stTitle, &Title::OnStopFinished, Qt::DirectConnection);
//    connect(VideoCtl::GetInstance(), &VideoCtl::SigStartPlay, &m_stTitle, &Title::OnPlay, Qt::DirectConnection);

    connect(&m_stCtrlBarAnimationTimer, &QTimer::timeout, this, &MainWindow::onCtrlBarAnimationTimeOut);

    connect(&m_stFullscreenMouseDetectTimer, &QTimer::timeout, this, &MainWindow::onFullscreenMouseDetectTimeOut);


    connect(&m_stActAbout, &QAction::triggered, this, &MainWindow::onShowAbout);
    connect(&m_stActFullscreen, &QAction::triggered, this, &MainWindow::onFullScreenPlay);
    connect(&m_stActExit, &QAction::triggered, this, &MainWindow::onCloseBtnClicked);
    connect(&m_stActOpen, &QAction::triggered, this, &MainWindow::openFile);
    


	return true;
}


void MainWindow::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())
	{
	case Qt::Key_Return://全屏
        onFullScreenPlay();
		break;
    case Qt::Key_Left://后退5s
        emit sigSeekBack();
        break;
    case Qt::Key_Right://前进5s
        emit sigSeekForward();
        break;
    case Qt::Key_Up://增加10音量
        emit sigAddVolume();
        break;
    case Qt::Key_Down://减少10音量
        emit sigSubVolume();
        break;
    case Qt::Key_Space://减少10音量
        emit sigPlayOrPause();
        break;
        
	default:
		break;
	}
}


void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        if (ui->TitleWid->geometry().contains(event->pos())) {
            m_bMoveDrag = true;
            m_DragPosition = event->globalPos() - this->pos();
        }
    }

    QWidget::mousePressEvent(event);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    m_bMoveDrag = false;

    QWidget::mouseReleaseEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (m_bMoveDrag) {
        move(event->globalPos() - m_DragPosition);
    }

    QWidget::mouseMoveEvent(event);
}

void MainWindow::contextMenuEvent(QContextMenuEvent* event)
{
    m_stMenu.exec(event->globalPos());
}

//定时器 滑动条显示
void MainWindow::timerEvent(QTimerEvent *e)
{
    Q_UNUSED(e)
    if (ui->CtrlBarWid->isPlaySliderPressed()) return;
    long long total = demuxThread.totalMs_;
    ui->CtrlBarWid->onVideoTotalSeconds(total);
    if (total > 0) {
        ui->CtrlBarWid->onVideoPlaySeconds(demuxThread.pts_);
    }
}

void MainWindow::onFullScreenPlay()
{
    if (m_bFullScreenPlay == false) {
        m_bFullScreenPlay = true;
        m_stActFullscreen.setChecked(true);
        //脱离父窗口后才能设置
        ui->VideoShow->setWindowFlags(Qt::Window);
        //多屏情况下，在当前屏幕全屏
        QScreen *pStCurScreen = qApp->screens().at(qApp->desktop()->screenNumber(this));
        ui->VideoShow->windowHandle()->setScreen(pStCurScreen);
        ui->VideoShow->showFullScreen();

        QRect stScreenRect = pStCurScreen->geometry();
        int nCtrlBarHeight = ui->CtrlBarWid->height();
        int nX = ui->VideoShow->x();
        m_stCtrlBarAnimationShow = QRect(nX, stScreenRect.height() - nCtrlBarHeight, stScreenRect.width(), nCtrlBarHeight);
        m_stCtrlBarAnimationHide = QRect(nX, stScreenRect.height(), stScreenRect.width(), nCtrlBarHeight);

        m_stCtrlbarAnimationShow->setStartValue(m_stCtrlBarAnimationHide);
        m_stCtrlbarAnimationShow->setEndValue(m_stCtrlBarAnimationShow);
        m_stCtrlbarAnimationShow->setDuration(1000);

        m_stCtrlbarAnimationHide->setStartValue(m_stCtrlBarAnimationShow);
        m_stCtrlbarAnimationHide->setEndValue(m_stCtrlBarAnimationHide);
        m_stCtrlbarAnimationHide->setDuration(1000);
        
        ui->CtrlBarWid->setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
        ui->CtrlBarWid->windowHandle()->setScreen(pStCurScreen);
        ui->CtrlBarWid->raise();
        ui->CtrlBarWid->setWindowOpacity(0.5);
        ui->CtrlBarWid->showNormal();
        ui->CtrlBarWid->windowHandle()->setScreen(pStCurScreen);
        
        m_stCtrlbarAnimationShow->start();
        m_bFullscreenCtrlBarShow = true;
        m_stFullscreenMouseDetectTimer.start();
    } else {
        m_bFullScreenPlay = false;
        m_stActFullscreen.setChecked(false);

        m_stCtrlbarAnimationShow->stop(); //快速切换时，动画还没结束导致控制面板消失
        m_stCtrlbarAnimationHide->stop();
        ui->CtrlBarWid->setWindowOpacity(1);
        ui->CtrlBarWid->setWindowFlags(Qt::SubWindow);
        
        ui->VideoShow->setWindowFlags(Qt::SubWindow);

        ui->CtrlBarWid->showNormal();
        ui->VideoShow->showNormal();

        m_stFullscreenMouseDetectTimer.stop();
    }
}

void MainWindow::onCtrlBarAnimationTimeOut()
{
    QApplication::setOverrideCursor(Qt::BlankCursor);
}

void MainWindow::onFullscreenMouseDetectTimeOut()
{
    if (m_bFullScreenPlay) {
        if (m_stCtrlBarAnimationShow.contains(cursor().pos())) {
            //判断鼠标是否在控制面板上面
            if (ui->CtrlBarWid->geometry().contains(cursor().pos())) {
                //继续显示
                m_bFullscreenCtrlBarShow = true;
            } else {
                //需要显示
                ui->CtrlBarWid->raise();
                
                m_stCtrlbarAnimationShow->start();
                m_stCtrlbarAnimationHide->stop();
                stCtrlBarHideTimer.stop();
            }
        } else {
            if (m_bFullscreenCtrlBarShow) {
                //需要隐藏
                m_bFullscreenCtrlBarShow = false;
                stCtrlBarHideTimer.singleShot(2000, this, &MainWindow::onCtrlBarHideTimeOut);
            }
        }
    }
}

void MainWindow::onCtrlBarHideTimeOut()
{
    m_stCtrlbarAnimationHide->start();
}

void MainWindow::onShowMenu()
{
    m_stMenu.exec(cursor().pos());
}

void MainWindow::onShowAbout()
{
    m_stAboutWidget.move(cursor().pos().x() - m_stAboutWidget.width()/2, cursor().pos().y() - m_stAboutWidget.height()/2);
    m_stAboutWidget.show();
}

void MainWindow::openFile()
{
    QString strFileName = QFileDialog::getOpenFileName(this,
                                                       "打开文件",
                                                       QDir::homePath(),
                                                       "视频文件(*.mkv *.rmvb *.mp4 *.avi *.flv *.wmv *.3gp)");

    emit sigOpenFile(strFileName);

    if (strFileName.isEmpty())return;
    m_stTitle.onPlay(strFileName);
    if (!demuxThread.open(strFileName.toLocal8Bit(), ui->VideoShow))
    {
        QMessageBox::information(0, "error", "open file failed!");
        return;
    }
    ui->CtrlBarWid->onPauseStat(demuxThread.isPause_);
}

void MainWindow::onCloseBtnClicked()
{
    this->close();
}

void MainWindow::onMinBtnClicked()
{
    this->showMinimized();
}

void MainWindow::onMaxBtnClicked()
{
    if (isMaximized()) {
        showNormal();
        emit sigShowMax(false);
    } else {
        showMaximized();
        emit sigShowMax(true);
    }
}

void MainWindow::onShowOrHidePlaylist()
{
     if (ui->PlaylistWid->isHidden()) {
         ui->PlaylistWid->show();
     } else {
         ui->PlaylistWid->hide();
     }
}

void MainWindow::onPlaySeek(double dPercent)
{
    demuxThread.seek(dPercent);
}

void MainWindow::onPlayOrPause()
{
    bool isPause = !demuxThread.isPause_;
    ui->CtrlBarWid->onPauseStat(isPause);
    demuxThread.setPause(isPause);
}
