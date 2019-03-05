#ifndef MEDIALIST_H
#define MEDIALIST_H

#include <QListWidget>
#include <QMenu>
#include <QAction>

class MediaList : public QListWidget
{
    Q_OBJECT

public:
    MediaList(QWidget *parent);
    ~MediaList();
    bool init();

protected:
    void contextMenuEvent(QContextMenuEvent* event);

private:
    void addFile();
    void removeFile();

signals:
    void sigAddFile(QString strFileName);

private:
    QMenu menu_;
    QAction actAdd_;     // 添加文件
    QAction actRemove_;  // 移除文件
    QAction actClearList_; // 清空列表
};

#endif // MEDIALIST_H


