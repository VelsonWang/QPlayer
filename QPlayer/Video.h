#ifndef VIDEO_H
#define VIDEO_H

#include <QObject>

#ifdef __cplusplus
extern "C"{
#endif



#ifdef __cplusplus
}
#endif


class Video : public QObject
{
    Q_OBJECT
public:
    explicit Video(QObject *parent = nullptr);

signals:

public slots:
};

#endif // VIDEO_H
