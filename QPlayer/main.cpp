#include <QApplication>
#include "XPlay2.h"


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

    XPlay2 player;
    player.show();

    return app.exec();
}
