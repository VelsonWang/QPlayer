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

    bool m_bPlaying; // 正在播放
    const int m_nShadowWidth; // 阴影宽度
    bool m_bFullScreenPlay; // 全屏播放标志

    QPropertyAnimation *m_stCtrlbarAnimationShow; // 全屏时控制面板浮动显示
    QPropertyAnimation *m_stCtrlbarAnimationHide; // 全屏时控制面板浮动显示
    QRect m_stCtrlBarAnimationShow; // 控制面板显示区域
    QRect m_stCtrlBarAnimationHide; // 控制面板隐藏区域

    QTimer m_stCtrlBarAnimationTimer;
    QTimer m_stFullscreenMouseDetectTimer; // 全屏时鼠标位置监测时钟
    bool m_bFullscreenCtrlBarShow;
    QTimer stCtrlBarHideTimer;

    PlayList m_stPlaylist;
    Title m_stTitle;

    bool m_bMoveDrag; //移动窗口标志
    QPoint m_DragPosition;

    About m_stAboutWidget;

    QMenu m_stMenu;
    QAction m_stActExit;
    QAction m_stActAbout;
    QAction m_stActOpen;
    QAction m_stActFullscreen;
};

#endif // MainWindow_H
