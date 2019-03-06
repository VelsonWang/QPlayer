#include "about.h"
#include "ui_About.h"
#include "GlobalHelper.h"


About::About(QWidget *parent)
    : QWidget(parent)
{
    ui = new Ui::About();
    ui->setupUi(this);
}

About::~About()
{
    delete ui;
}

bool About::init()
{
    this->setWindowModality(Qt::ApplicationModal);

    this->setWindowIcon(QIcon(":/Resources/player.png"));
    QPixmap pixmap = QPixmap(":/Resources/player.png").scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->LogoLabel->setPixmap(pixmap);

    setStyleSheet(GlobalHelper::getQssStr(":/Resources/qss/about.css"));

    QString strVersion = QString("版本：%1\n时间：%2")
            .arg(GlobalHelper::getAppVersion())
            .arg(QString(__DATE__) + " " + QString(__TIME__));

    ui->VersionLabel->setText(strVersion);

    return true;
}



void About::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        bMoveDrag_ = true;
        dragPosition_ = event->globalPos() - this->pos();
    }

    QWidget::mousePressEvent(event);
}

void About::mouseReleaseEvent(QMouseEvent *event)
{
    bMoveDrag_ = false;
    QWidget::mouseReleaseEvent(event);
}

void About::mouseMoveEvent(QMouseEvent *event)
{
    if (bMoveDrag_) {
        move(event->globalPos() - dragPosition_);
    }

    QWidget::mouseMoveEvent(event);
}


void About::on_ClosePushButton_clicked()
{
    hide();
}
