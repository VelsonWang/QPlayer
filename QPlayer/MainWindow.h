#ifndef MainWindow_H
#define MainWindow_H

#include <QWidget>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QMenu>
#include <QAction>
#include <QPropertyAnimation>
#include <QTimer>
#include <QMainWindow>

#include "PlayList.h"
#include "Title.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QMainWindow *parent = 0);
    ~MainWindow();

    bool init();

protected:
    void paintEvent(QPaintEvent *event);

//     //窗口大小变化事件
//     void resizeEvent(QResizeEvent *event);
//     //窗口移动事件
//     void moveEvent(QMoveEvent *event);

    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    //void dragEnterEvent(QDragEnterEvent *event);
    //void dragMoveEvent(QDragMoveEvent *event);
    //void dropEvent(QDropEvent *event);

    //按键事件
    void keyPressEvent(QKeyEvent *event);

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    void contextMenuEvent(QContextMenuEvent* event);
    // 定时器 滑动条显示
    void timerEvent(QTimerEvent *e);

private:
    bool connectSignalSlots();

    // 关闭、最小化、最大化按钮响应
    void onCloseBtnClicked();
    void onMinBtnClicked();
    void onMaxBtnClicked();
    // 显示、隐藏播放列表
    void onShowOrHidePlaylist();

    void onFullScreenPlay();

    void onCtrlBarAnimationTimeOut();
    void onFullscreenMouseDetectTimeOut();

    void onCtrlBarHideTimeOut();
    void onShowMenu();
    void onShowAbout();
    void openFile();


private slots:
    void onPlaySeek(double dPercent);
    void onPlayOrPause();
    // 播放文件
    void onPlayVideo(const QString &fileName);
    // 停止播放
    void onStopPlay();

signals:
    // 最大化信号
    void sigShowMax(bool bIfMax);
    void sigSeekForward();
    void sigSeekBack();
    void sigAddVolume();
    void sigSubVolume();
    void sigPlayOrPause();
    void sigOpenFile(QString strFilename);

private:
    Ui::MainWindow *ui;

    bool bPlaying_; // 正在播放
    const int nShadowWidth_; // 阴影宽度
    bool bFullScreenPlay_; // 全屏播放标志

    QPropertyAnimation *ctrlbarAnimationShow_; // 全屏时控制面板浮动显示
    QPropertyAnimation *ctrlbarAnimationHide_; // 全屏时控制面板浮动显示
    QRect ctrlBarShowRect_; // 控制面板显示区域
    QRect ctrlBarHideRect_; // 控制面板隐藏区域

    QTimer ctrlBarAnimationTimer_;
    QTimer fullscreenMouseDetectTimer_; // 全屏时鼠标位置监测时钟
    bool bFullscreenCtrlBarShow_;
    QTimer ctrlBarHideTimer_;

    PlayList playlist_;
    Title title_;

    bool bMoveDrag_; //移动窗口标志
    QPoint dragPosition_;

    About aboutWidget_;

    QMenu menu_;
    QAction actExit_;
    QAction actAbout_;
    QAction actOpen_;
    QAction actFullscreen_;
};

#endif // MainWindow_H
