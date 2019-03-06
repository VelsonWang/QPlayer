#include <QApplication>
#include <QFontDatabase>
#include "MainWindow.h"

#ifdef __cplusplus
extern "C"{
#endif

#include <SDL.h>
#include <SDL_thread.h>

#ifdef __cplusplus
}
#endif

#ifdef __MINGW32__
#undef main /* Prevents SDL from overriding main() */
#endif

using namespace std;


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QFontDatabase::addApplicationFont(":/Resources/fontawesome-webfont.ttf");
    //QFontDatabase::addApplicationFont(":/Resources/fa-solid-900.ttf");

    MainWindow mainWin;
    if (mainWin.init() == false)
    {
        return -1;
    }
    mainWin.show();

    return app.exec();
}
