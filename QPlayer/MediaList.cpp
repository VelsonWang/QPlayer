#include <QContextMenuEvent>
#include <QFileDialog>
#include "MediaList.h"


MediaList::MediaList(QWidget *parent)
    : QListWidget(parent),
    menu_(this)
{
}

MediaList::~MediaList()
{
}

bool MediaList::init()
{
    actAdd_.setText("添加");
    menu_.addAction(&actAdd_);
    actRemove_.setText("移除所选项");
    QMenu* stRemoveMenu = menu_.addMenu("移除");
    stRemoveMenu->addAction(&actRemove_);
    actClearList_.setText("清空列表");
    menu_.addAction(&actClearList_);

    connect(&actAdd_, &QAction::triggered, this, &MediaList::addFile);
    connect(&actRemove_, &QAction::triggered, this, &MediaList::removeFile);
    connect(&actClearList_, &QAction::triggered, this, &QListWidget::clear);

    return true;
}

void MediaList::contextMenuEvent(QContextMenuEvent* event)
{
    menu_.exec(event->globalPos());
}

void MediaList::addFile()
{
    QStringList listFileName = QFileDialog::getOpenFileNames(this,
                                                             "打开文件",
                                                             QDir::homePath(),
                                                             "视频文件(*.mkv *.rmvb *.mp4 *.avi *.flv *.wmv *.3gp)");

    for (QString strFileName : listFileName) {
        emit sigAddFile(strFileName);
    }
}

void MediaList::removeFile()
{
    takeItem(currentRow());
}

