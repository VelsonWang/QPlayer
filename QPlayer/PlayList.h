#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QWidget>
#include <QListWidgetItem>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QMimeData>

namespace Ui {
class PlayList;
}

class PlayList : public QWidget
{
    Q_OBJECT

public:
    explicit PlayList(QWidget *parent = 0);
    ~PlayList();

    bool init();

	/**
	 * @brief	获取播放列表状态
	 * 
	 * @return	true 显示 false 隐藏
	 * @note 	
	 */
    bool getPlaylistStatus();

public:
    void onAddFile(QString strFileName);
    void onAddFileAndPlay(QString strFileName);

    void onBackwardPlay();
    void onForwardPlay();

    // 在这里定义dock的初始大小
    QSize sizeHint() const
    {
        return QSize(150, 900);
    }

protected:
    void dropEvent(QDropEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);

signals:
    void sigUpdateUi();	// 界面排布更新
    void sigPlay(QString strFile); // 播放文件

private:
    bool initUi();
    bool connectSignalSlots();
    
private slots:
	void on_List_itemDoubleClicked(QListWidgetItem *item);

private:
    Ui::PlayList *ui;
    int currentPlayListIndex_;
};

#endif // PLAYLIST_H
