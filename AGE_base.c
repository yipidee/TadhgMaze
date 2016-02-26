#ifdef __linux
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_image.h>
    #include <SDL2/SDL_ttf.h>
#elif _WIN32
    #include <SDL.h>
    #include <SDL_image.h>
    #include <SDL_ttf.h>
#endif
#include "AGE_base.h"

//init AGE base module
void AGE_init()
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" );
    IMG_Init(IMG_INIT_JPG);
    TTF_Init();
}

//init AGE base module
void AGE_quit()
{
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}
