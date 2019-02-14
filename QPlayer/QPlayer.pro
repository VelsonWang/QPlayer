
QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT += opengl
QT += multimedia

# use C++11 in Project
CONFIG += c++11

TARGET = QPlayer
TEMPLATE = app

DESTDIR = $$OUT_PWD/../bin

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += $$PWD/ffmpeg/include \
               $$PWD/SDL2/include \

LIBS += $$PWD/ffmpeg/lib/avcodec.lib \
        $$PWD/ffmpeg/lib/avdevice.lib \
        $$PWD/ffmpeg/lib/avfilter.lib \
        $$PWD/ffmpeg/lib/avformat.lib \
        $$PWD/ffmpeg/lib/avutil.lib \
        $$PWD/ffmpeg/lib/postproc.lib \
        $$PWD/ffmpeg/lib/swresample.lib \
        $$PWD/ffmpeg/lib/swscale.lib \
        $$PWD/SDL2/lib/x86/SDL2.lib \
        $$PWD/SDL2/lib/x86/SDL2_mixer.lib \
#        $$PWD/SDL2/lib/x86/SDL2main.lib

SOURCES += \
        main.cpp \
    XDemux.cpp \
    XDecode.cpp \
    XVideoWidget.cpp \
    XPlay2.cpp \
    XResample.cpp \
    XAudioPlay.cpp \
    XSlider.cpp \
    XAudioThread.cpp \
    XDecodeThread.cpp \
    XVideoThread.cpp \
    XDemuxThread.cpp

HEADERS += \
    XDemux.h \
    XDecode.h \
    XVideoWidget.h \
    XPlay2.h \
    XResample.h \
    XAudioPlay.h \
    IVideoCall.h \
    XSlider.h \
    XAudioThread.h \
    XDecodeThread.h \
    XVideoThread.h \
    XDemuxThread.h

FORMS += \
    XPlay2.ui

RESOURCES += \
    XPlay2.qrc
