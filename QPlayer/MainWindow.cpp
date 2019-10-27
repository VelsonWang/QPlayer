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
      nShadowWidth_(0),
      menu_(this),
      playlist_(this),
      title_(this),
      bMoveDrag_(false) {
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

    bPlaying_ = false;

    bFullScreenPlay_ = false;

    ctrlBarAnimationTimer_.setInterval(1000);
    fullscreenMouseDetectTimer_.setInterval(FULLSCREEN_MOUSE_DETECT_TIME);

    startTimer(40);
}

MainWindow::~MainWindow() {
    demuxThread.close();
    delete ui;
}

bool MainWindow::init() {
    QWidget *em = new QWidget(this);
    ui->PlaylistWid->setTitleBarWidget(em);
    ui->PlaylistWid->setWidget(&playlist_);

    QWidget *emTitle = new QWidget(this);
    ui->TitleWid->setTitleBarWidget(emTitle);
    ui->TitleWid->setWidget(&title_);
    
    //连接自定义信号与槽
    if (connectSignalSlots() == false) {
        return false;
    }

    if (ui->CtrlBarWid->init() == false ||
        playlist_.init() == false ||
        title_.init() == false) {
        return false;
    }

    ctrlbarAnimationShow_ = new QPropertyAnimation(ui->CtrlBarWid, "geometry");
    ctrlbarAnimationHide_ = new QPropertyAnimation(ui->CtrlBarWid, "geometry");

    if (aboutWidget_.init() == false) {
        return false;
    }

    actFullscreen_.setText(tr("全屏"));
    actFullscreen_.setCheckable(true);
    menu_.addAction(&actFullscreen_);

    actOpen_.setText(tr("打开"));
    menu_.addAction(&actOpen_);

    actAbout_.setText(tr("关于"));
    menu_.addAction(&actAbout_);
    
    actExit_.setText(tr("退出"));
    menu_.addAction(&actExit_);

    return true;
}

void MainWindow::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
}


void MainWindow::enterEvent(QEvent *event) {
    Q_UNUSED(event);
}

void MainWindow::leaveEvent(QEvent *event) {
    Q_UNUSED(event);
}

bool MainWindow::connectSignalSlots() {
    connect(&title_, &Title::sigCloseBtnClicked, this, &MainWindow::onCloseBtnClicked);
    connect(&title_, &Title::sigMaxBtnClicked, this, &MainWindow::onMaxBtnClicked);
    connect(&title_, &Title::sigMinBtnClicked, this, &MainWindow::onMinBtnClicked);
    connect(&title_, &Title::sigDoubleClicked, this, &MainWindow::onMaxBtnClicked);
    connect(&title_, &Title::sigFullScreenBtnClicked, this, &MainWindow::onFullScreenPlay);
    connect(&title_, &Title::sigOpenFile, &playlist_, &PlayList::onAddFileAndPlay);
    connect(&title_, &Title::sigShowMenu, this, &MainWindow::onShowMenu);
    
    connect(&playlist_, &PlayList::sigPlay, this, &MainWindow::onPlayVideo);

//    connect(ui->VideoShow, &Show::sigOpenFile, &playlist_, &Playlist::OnAddFileAndPlay);
//    connect(ui->ShowWid, &Show::SigFullScreen, this, &MainWindow::OnFullScreenPlay);
//    connect(ui->ShowWid, &Show::SigPlayOrPause, VideoCtl::GetInstance(), &VideoCtl::OnPause);
//    connect(ui->ShowWid, &Show::SigStop, VideoCtl::GetInstance(), &VideoCtl::OnStop);
//    connect(ui->ShowWid, &Show::SigShowMenu, this, &MainWindow::OnShowMenu);

    connect(ui->CtrlBarWid, &CtrlBar::sigShowOrHidePlaylist, this, &MainWindow::onShowOrHidePlaylist);
    connect(ui->CtrlBarWid, &CtrlBar::sigPlaySeek, this, &MainWindow::onPlaySeek);
    connect(ui->CtrlBarWid, &CtrlBar::sigPlayVolume, this, &MainWindow::onSetVolume);
    connect(ui->CtrlBarWid, &CtrlBar::sigPlayOrPause, this, &MainWindow::onPlayOrPause);
    connect(ui->CtrlBarWid, &CtrlBar::sigStop, this, &MainWindow::onStopPlay);
    connect(ui->CtrlBarWid, &CtrlBar::sigBackwardPlay, &playlist_, &PlayList::onBackwardPlay);
    connect(ui->CtrlBarWid, &CtrlBar::sigForwardPlay, &playlist_, &PlayList::onForwardPlay);
    connect(ui->CtrlBarWid, &CtrlBar::sigShowMenu, this, &MainWindow::onShowMenu);
//    connect(ui->CtrlBarWid, &CtrlBar::SigShowSetting, this, &MainWindow::OnShowSettingWid);

    connect(this, &MainWindow::sigShowMax, &title_, &Title::onChangeMaxBtnStyle);
//    connect(this, &MainWindow::SigSeekForward, VideoCtl::GetInstance(), &VideoCtl::OnSeekForward);
//    connect(this, &MainWindow::SigSeekBack, VideoCtl::GetInstance(), &VideoCtl::OnSeekBack);
//    connect(this, &MainWindow::SigAddVolume, VideoCtl::GetInstance(), &VideoCtl::OnAddVolume);
//    connect(this, &MainWindow::SigSubVolume, VideoCtl::GetInstance(), &VideoCtl::OnSubVolume);
    connect(this, &MainWindow::sigOpenFile, &playlist_, &PlayList::onAddFileAndPlay);
    
    
//    connect(VideoCtl::GetInstance(), &VideoCtl::SigVideoTotalSeconds, ui->CtrlBarWid, &CtrlBar::OnVideoTotalSeconds);
//    connect(VideoCtl::GetInstance(), &VideoCtl::SigVideoPlaySeconds, ui->CtrlBarWid, &CtrlBar::OnVideoPlaySeconds);
//    connect(VideoCtl::GetInstance(), &VideoCtl::SigVideoVolume, ui->CtrlBarWid, &CtrlBar::OnVideopVolume);
//    connect(VideoCtl::GetInstance(), &VideoCtl::SigPauseStat, ui->CtrlBarWid, &CtrlBar::OnPauseStat);
//    connect(VideoCtl::GetInstance(), &VideoCtl::SigStopFinished, ui->CtrlBarWid, &CtrlBar::OnStopFinished, Qt::DirectConnection);
//    connect(VideoCtl::GetInstance(), &VideoCtl::SigStopFinished, ui->ShowWid, &Show::OnStopFinished, Qt::DirectConnection);
//    connect(VideoCtl::GetInstance(), &VideoCtl::SigStopFinished, &title_, &Title::OnStopFinished, Qt::DirectConnection);
//    connect(VideoCtl::GetInstance(), &VideoCtl::SigStartPlay, &title_, &Title::OnPlay, Qt::DirectConnection);

    connect(&ctrlBarAnimationTimer_, &QTimer::timeout, this, &MainWindow::onCtrlBarAnimationTimeOut);
    connect(&fullscreenMouseDetectTimer_, &QTimer::timeout, this, &MainWindow::onFullscreenMouseDetectTimeOut);

    connect(&actAbout_, &QAction::triggered, this, &MainWindow::onShowAbout);
    connect(&actFullscreen_, &QAction::triggered, this, &MainWindow::onFullScreenPlay);
    connect(&actExit_, &QAction::triggered, this, &MainWindow::onCloseBtnClicked);
    connect(&actOpen_, &QAction::triggered, this, &MainWindow::openFile);
    
	return true;
}


void MainWindow::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
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


void MainWindow::mousePressEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        if (ui->TitleWid->geometry().contains(event->pos())) {
            bMoveDrag_ = true;
            dragPosition_ = event->globalPos() - this->pos();
        }
    }
    QWidget::mousePressEvent(event);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event) {
    bMoveDrag_ = false;
    QWidget::mouseReleaseEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
    if (bMoveDrag_) {
        move(event->globalPos() - dragPosition_);
    }
    QWidget::mouseMoveEvent(event);
}

void MainWindow::contextMenuEvent(QContextMenuEvent* event) {
    menu_.exec(event->globalPos());
}


/**
 * @brief MainWindow::timerEvent
 * @details 定时器 滑动条显示
 * @param e
 */
void MainWindow::timerEvent(QTimerEvent *e) {
    Q_UNUSED(e)
    if (ui->CtrlBarWid->isPlaySliderPressed())
        return;
    long long total = demuxThread.totalMs_;
    ui->CtrlBarWid->onVideoTotalSeconds(total/1000);
    if (total > 0) {
        ui->CtrlBarWid->onVideoPlaySeconds(demuxThread.pts_/1000);
    }
}

void MainWindow::onFullScreenPlay() {
    if (bFullScreenPlay_ == false) {
        bFullScreenPlay_ = true;
        actFullscreen_.setChecked(true);
        //脱离父窗口后才能设置
        ui->VideoShow->setWindowFlags(Qt::Window);
        //多屏情况下，在当前屏幕全屏
        QScreen *pStCurScreen = qApp->screens().at(qApp->desktop()->screenNumber(this));
        ui->VideoShow->windowHandle()->setScreen(pStCurScreen);
        ui->VideoShow->showFullScreen();

        QRect stScreenRect = pStCurScreen->geometry();
        int nCtrlBarHeight = ui->CtrlBarWid->height();
        int nX = ui->VideoShow->x();
        ctrlBarShowRect_ = QRect(nX, stScreenRect.height() - nCtrlBarHeight, stScreenRect.width(), nCtrlBarHeight);
        ctrlBarHideRect_ = QRect(nX, stScreenRect.height(), stScreenRect.width(), nCtrlBarHeight);

        ctrlbarAnimationShow_->setStartValue(ctrlBarHideRect_);
        ctrlbarAnimationShow_->setEndValue(ctrlBarShowRect_);
        ctrlbarAnimationShow_->setDuration(1000);

        ctrlbarAnimationHide_->setStartValue(ctrlBarShowRect_);
        ctrlbarAnimationHide_->setEndValue(ctrlBarHideRect_);
        ctrlbarAnimationHide_->setDuration(1000);
        
        ui->CtrlBarWid->setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
        ui->CtrlBarWid->windowHandle()->setScreen(pStCurScreen);
        ui->CtrlBarWid->raise();
        ui->CtrlBarWid->setWindowOpacity(0.5);
        ui->CtrlBarWid->showNormal();
        ui->CtrlBarWid->windowHandle()->setScreen(pStCurScreen);
        
        ctrlbarAnimationShow_->start();
        bFullscreenCtrlBarShow_ = true;
        fullscreenMouseDetectTimer_.start();
    } else {
        bFullScreenPlay_ = false;
        actFullscreen_.setChecked(false);

        ctrlbarAnimationShow_->stop(); //快速切换时，动画还没结束导致控制面板消失
        ctrlbarAnimationHide_->stop();
        ui->CtrlBarWid->setWindowOpacity(1);
        ui->CtrlBarWid->setWindowFlags(Qt::SubWindow);
        
        ui->VideoShow->setWindowFlags(Qt::SubWindow);

        ui->CtrlBarWid->showNormal();
        ui->VideoShow->showNormal();

        fullscreenMouseDetectTimer_.stop();
    }
}

void MainWindow::onCtrlBarAnimationTimeOut() {
    QApplication::setOverrideCursor(Qt::BlankCursor);
}

void MainWindow::onFullscreenMouseDetectTimeOut() {
    if (bFullScreenPlay_) {
        if (ctrlBarShowRect_.contains(cursor().pos())) {
            //判断鼠标是否在控制面板上面
            if (ui->CtrlBarWid->geometry().contains(cursor().pos())) {
                //继续显示
                bFullscreenCtrlBarShow_ = true;
            } else {
                //需要显示
                ui->CtrlBarWid->raise();
                
                ctrlbarAnimationShow_->start();
                ctrlbarAnimationHide_->stop();
                ctrlBarHideTimer_.stop();
            }
        } else {
            if (bFullscreenCtrlBarShow_) {
                //需要隐藏
                bFullscreenCtrlBarShow_ = false;
                ctrlBarHideTimer_.singleShot(2000, this, &MainWindow::onCtrlBarHideTimeOut);
            }
        }
    }
}

void MainWindow::onCtrlBarHideTimeOut() {
    ctrlbarAnimationHide_->start();
}

void MainWindow::onShowMenu() {
    menu_.exec(cursor().pos());
}

void MainWindow::onShowAbout() {
    aboutWidget_.move(cursor().pos().x() - aboutWidget_.width()/2, cursor().pos().y() - aboutWidget_.height()/2);
    aboutWidget_.show();
}

void MainWindow::openFile() {
    QString strFileName = QFileDialog::getOpenFileName(this,
                                                       "打开文件",
                                                       QDir::homePath(),
                                                       "视频文件(*.mkv *.rmvb *.mp4 *.avi *.flv *.wmv *.3gp)");

    emit sigOpenFile(strFileName);

    if (strFileName.isEmpty())
        return;

    onPlayVideo(strFileName);
}


/**
 * @brief MainWindow::onPlayVideo
 * @details 播放视频文件
 * @param fileName 视频文件
 */
void MainWindow::onPlayVideo(const QString &fileName) {
    demuxThread.isExit_ = false;
    demuxThread.start();
    bPlaying_ = true;
    title_.onPlay(fileName);
    if (!demuxThread.open(fileName.toLocal8Bit(), ui->VideoShow)) {
        QMessageBox::information(0, tr("错误"), tr("打开文件错误！"));
        return;
    }
    ui->CtrlBarWid->onPauseStat(demuxThread.isPause_);
}

void MainWindow::onCloseBtnClicked() {
    this->close();
}

void MainWindow::onMinBtnClicked() {
    this->showMinimized();
}

void MainWindow::onMaxBtnClicked() {
    if (isMaximized()) {
        showNormal();
        emit sigShowMax(false);
    } else {
        showMaximized();
        emit sigShowMax(true);
    }
}

void MainWindow::onShowOrHidePlaylist() {
     if (ui->PlaylistWid->isHidden()) {
         ui->PlaylistWid->show();
     } else {
         ui->PlaylistWid->hide();
     }
}

void MainWindow::onPlaySeek(double dPercent) {
    demuxThread.seek(dPercent);
}

void MainWindow::onPlayOrPause() {
    if(!bPlaying_)
        return;
    bool isPause = !demuxThread.isPause_;
    ui->CtrlBarWid->onPauseStat(isPause);
    demuxThread.setPause(isPause);
}


/**
 * @brief MainWindow::onStopPlay
 * @details 停止播放
 */
void MainWindow::onStopPlay() {
    demuxThread.isExit_ = true;
    bPlaying_ = false;
    ui->VideoShow->clearWidget();
}

/**
 * @brief MainWindow::onSetVolume
 * @details 设置音量
 * @param volume
 */
void MainWindow::onSetVolume(float volume) {
    demuxThread.setVolume(volume);
}
