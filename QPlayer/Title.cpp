
#include <QPainter>
#include <QFileInfo>
#include <QFontMetrics>
#include <QMessageBox>
#include <QFileDialog>

#include "Title.h"
#include "ui_Title.h"

#include "GlobalHelper.h"


Title::Title(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Title),
    stActionGroup_(this),
    stMenu_(this)
{
    ui->setupUi(this);

    connect(ui->CloseBtn, &QPushButton::clicked, this, &Title::sigCloseBtnClicked);
    connect(ui->MinBtn, &QPushButton::clicked, this, &Title::sigMinBtnClicked);
    connect(ui->MaxBtn, &QPushButton::clicked, this, &Title::sigMaxBtnClicked);
    connect(ui->FullScreenBtn, &QPushButton::clicked, this, &Title::sigFullScreenBtnClicked);
    connect(ui->MenuBtn, &QPushButton::clicked, this, &Title::sigShowMenu);
 
    stMenu_.addAction("最大化", this, &Title::sigMaxBtnClicked);
    stMenu_.addAction("最小化", this, &Title::sigMinBtnClicked);
    stMenu_.addAction("退出", this, &Title::sigCloseBtnClicked);

    QMenu* stMenu = stMenu_.addMenu("打开");
    stMenu->addAction("打开文件", this, &Title::openFile);

    ui->MenuBtn->setToolTip("显示主菜单");
    ui->MinBtn->setToolTip("最小化");
    ui->MaxBtn->setToolTip("最大化");
    ui->CloseBtn->setToolTip("关闭");
    ui->FullScreenBtn->setToolTip("全屏");
}

Title::~Title()
{
    delete ui;
}

bool Title::init()
{
    if (initUi() == false) {
        return false;
    }

    return true;
}

bool Title::initUi()
{
    ui->MovieNameLab->clear();

    //保证窗口不被绘制上的部分透明
    setAttribute(Qt::WA_TranslucentBackground);

    setStyleSheet(GlobalHelper::getQssStr(":/Resources/qss/Title.css"));

    GlobalHelper::setIcon(ui->MaxBtn, 9, QChar(0xf2d0));
    GlobalHelper::setIcon(ui->MinBtn, 9, QChar(0xf2d1));
    GlobalHelper::setIcon(ui->CloseBtn, 9, QChar(0xf00d));
    GlobalHelper::setIcon(ui->FullScreenBtn, 9, QChar(0xf065));

    if (about_.init() == false) {
        return false;
    }

    return true;
}

void Title::openFile()
{
    QString strFileName = QFileDialog::getOpenFileName(this,
                                                       "打开文件",
                                                       QDir::homePath(),
                                                       "视频文件(*.mkv *.rmvb *.mp4 *.avi *.flv *.wmv *.3gp)");

    emit sigOpenFile(strFileName);
}

void Title::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
}

void Title::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton) {
        emit sigDoubleClicked();
    }
}

void Title::resizeEvent(QResizeEvent *event)
{
    //ChangeMovieNameShow();
}



void Title::changeMovieNameShow()
{
    QFontMetrics fontMetrics(ui->MovieNameLab->font());
    int fontSize = fontMetrics.width(strMovieName_);
    int showwidth = ui->MovieNameLab->width();
    if (fontSize > showwidth) {
        QString str = fontMetrics.elidedText(strMovieName_, Qt::ElideRight, ui->MovieNameLab->width());//返回一个带有省略号的字符串
        ui->MovieNameLab->setText(str);
    } else {
        ui->MovieNameLab->setText(strMovieName_);
    }
}

void Title::onChangeMaxBtnStyle(bool bIfMax)
{
    if (bIfMax) {
        GlobalHelper::setIcon(ui->MaxBtn, 9, QChar(0xf2d2));
        ui->MaxBtn->setToolTip("还原");
    } else {
        GlobalHelper::setIcon(ui->MaxBtn, 9, QChar(0xf2d0));
        ui->MaxBtn->setToolTip("最大化");
    }
}

void Title::onPlay(QString strMovieName)
{
    qDebug() << "Title::OnPlay";
    QFileInfo fileInfo(strMovieName);
    strMovieName_ = fileInfo.fileName();
    ui->MovieNameLab->setText(strMovieName_);
    //ChangeMovieNameShow();
}

void Title::onStopFinished()
{
    qDebug() << "Title::OnStopFinished";
    ui->MovieNameLab->clear();
}

