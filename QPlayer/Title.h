
#ifndef TITLE_H
#define TITLE_H

#include <QWidget>
#include <QMouseEvent>
#include <QMenu>
#include <QActionGroup>
#include <QAction>

#include "about.h"

namespace Ui {
class Title;
}

class Title : public QWidget
{
    Q_OBJECT

public:
    explicit Title(QWidget *parent = 0);
    ~Title();
    bool init();

private:
    void paintEvent(QPaintEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *event);
    void changeMovieNameShow();
    bool initUi();
    void showAbout();
    void openFile();

public slots:
    void onChangeMaxBtnStyle(bool bIfMax);
    void onPlay(QString strMovieName);
    void onStopFinished();

signals:
    void sigCloseBtnClicked();	// 点击关闭按钮
    void sigMinBtnClicked();	// 点击最小化按钮
    void sigMaxBtnClicked();	// 点击最大化按钮
    void sigDoubleClicked();	// 双击标题栏
    void sigFullScreenBtnClicked(); // 点击全屏按钮
    void sigOpenFile(QString strFileName); // 打开文件
    void sigShowMenu();

private:
    Ui::Title *ui;
    QString strMovieName_;
    QMenu stMenu_;
    QActionGroup stActionGroup_;
    About about_;
};

#endif // MAINWIDTITLE_H
