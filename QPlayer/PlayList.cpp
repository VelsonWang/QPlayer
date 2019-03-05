#include <QDebug>
#include <QDir>

#include "playlist.h"
#include "ui_playlist.h"
#include "GlobalHelper.h"


PlayList::PlayList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlayList)
{
    ui->setupUi(this);
	
}

PlayList::~PlayList()
{
    QStringList strListPlayList;
    for (int i = 0; i < ui->List->count(); i++) {
        strListPlayList.append(ui->List->item(i)->toolTip());
    }
    GlobalHelper::savePlaylist(strListPlayList);

    delete ui;
}

bool PlayList::init()
{
    if (ui->List->init() == false) {
        return false;
    }

    if (initUi() == false) {
        return false;
    }

    if (connectSignalSlots() == false) {
        return false;
    }

    setAcceptDrops(true);

	return true;
}

bool PlayList::initUi()
{
    setStyleSheet(GlobalHelper::getQssStr(":/Resources/qss/playlist.css"));
    ui->List->clear();

    QStringList strListPlaylist;
    GlobalHelper::getPlaylist(strListPlaylist);

    for (QString strVideoFile : strListPlaylist) {
        QFileInfo fileInfo(strVideoFile);
        if (fileInfo.exists()) {
            QListWidgetItem *pItem = new QListWidgetItem(ui->List);
            pItem->setData(Qt::UserRole, QVariant(fileInfo.filePath()));  // 用户数据
            pItem->setText(QString("%1").arg(fileInfo.fileName()));  // 显示文本
            pItem->setToolTip(fileInfo.filePath());
            ui->List->addItem(pItem);
        }
    }

    if (strListPlaylist.length() > 0) {
        ui->List->setCurrentRow(0);
    }

    return true;
}

bool PlayList::connectSignalSlots()
{
	QList<bool> listRet;
	bool bRet;

    bRet = connect(ui->List, &MediaList::sigAddFile, this, &PlayList::onAddFile);
    listRet.append(bRet);

    for (bool bReturn : listRet) {
        if (bReturn == false) {
			return false;
		}
	}

	return true;
}

void PlayList::on_List_itemDoubleClicked(QListWidgetItem *item)
{
    emit sigPlay(item->data(Qt::UserRole).toString());
    currentPlayListIndex_ = ui->List->row(item);
    ui->List->setCurrentRow(currentPlayListIndex_);
}

bool PlayList::getPlaylistStatus()
{
    if (this->isHidden()) {
        return false;
    }

    return true;
}

void PlayList::onAddFile(QString strFileName)
{
    bool bSupportMovie = strFileName.endsWith(".mkv", Qt::CaseInsensitive) ||
        strFileName.endsWith(".rmvb", Qt::CaseInsensitive) ||
        strFileName.endsWith(".mp4", Qt::CaseInsensitive) ||
        strFileName.endsWith(".avi", Qt::CaseInsensitive) ||
        strFileName.endsWith(".flv", Qt::CaseInsensitive) ||
        strFileName.endsWith(".wmv", Qt::CaseInsensitive) ||
        strFileName.endsWith(".3gp", Qt::CaseInsensitive);
    if (!bSupportMovie) {
        return;
    }


    QFileInfo fileInfo(strFileName);
	QList<QListWidgetItem *> listItem = ui->List->findItems(fileInfo.fileName(), Qt::MatchExactly);
    QListWidgetItem *pItem = nullptr;
    if (listItem.isEmpty()) {
        pItem = new QListWidgetItem(ui->List);
        pItem->setData(Qt::UserRole, QVariant(fileInfo.filePath()));  // 用户数据
        pItem->setText(fileInfo.fileName());  // 显示文本
        pItem->setToolTip(fileInfo.filePath());
        ui->List->addItem(pItem);
    } else {
        pItem = listItem.at(0);
    }
}

void PlayList::onAddFileAndPlay(QString strFileName)
{
    bool bSupportMovie = strFileName.endsWith(".mkv", Qt::CaseInsensitive) ||
        strFileName.endsWith(".rmvb", Qt::CaseInsensitive) ||
        strFileName.endsWith(".mp4", Qt::CaseInsensitive) ||
        strFileName.endsWith(".avi", Qt::CaseInsensitive) ||
        strFileName.endsWith(".flv", Qt::CaseInsensitive) ||
        strFileName.endsWith(".wmv", Qt::CaseInsensitive) ||
        strFileName.endsWith(".3gp", Qt::CaseInsensitive);
    if (!bSupportMovie) {
        return;
    }

    QFileInfo fileInfo(strFileName);
    QList<QListWidgetItem *> listItem = ui->List->findItems(fileInfo.fileName(), Qt::MatchExactly);
    QListWidgetItem *pItem = nullptr;
    if (listItem.isEmpty()) {
        pItem = new QListWidgetItem(ui->List);
        pItem->setData(Qt::UserRole, QVariant(fileInfo.filePath()));  // 用户数据
        pItem->setText(fileInfo.fileName());  // 显示文本
        pItem->setToolTip(fileInfo.filePath());
        ui->List->addItem(pItem);
    } else {
        pItem = listItem.at(0);
    }
    on_List_itemDoubleClicked(pItem);
}

void PlayList::onBackwardPlay()
{
    if (currentPlayListIndex_ == 0) {
        currentPlayListIndex_ = ui->List->count() - 1;
        on_List_itemDoubleClicked(ui->List->item(currentPlayListIndex_));
        ui->List->setCurrentRow(currentPlayListIndex_);
    } else {
        currentPlayListIndex_--;
        on_List_itemDoubleClicked(ui->List->item(currentPlayListIndex_));
        ui->List->setCurrentRow(currentPlayListIndex_);
    }
}

void PlayList::onForwardPlay()
{
    if (currentPlayListIndex_ == ui->List->count() - 1) {
        currentPlayListIndex_ = 0;
        on_List_itemDoubleClicked(ui->List->item(currentPlayListIndex_));
        ui->List->setCurrentRow(currentPlayListIndex_);
    } else {
        currentPlayListIndex_++;
        on_List_itemDoubleClicked(ui->List->item(currentPlayListIndex_));
        ui->List->setCurrentRow(currentPlayListIndex_);
    }
}

void PlayList::dropEvent(QDropEvent *event)
{
    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty()) {
        return;
    }

    for (QUrl url : urls) {
        QString strFileName = url.toLocalFile();

        onAddFile(strFileName);
    }
}

void PlayList::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}
