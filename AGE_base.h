#ifndef AGE_BASE_H
#define AGE_BASE_H

#ifdef __linux
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_image.h>
#elif _WIN32
    #include <SDL.h>
    #include <SDL_image.h>
#endif

#define AGE_getWindow(w, h) SDL_CreateWindow("Tadhg Maze Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_SHOWN)
#define AGE_getRenderer(window) SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE)
#define AGE_releaseRenderer(r) SDL_DestroyRenderer(r)
#define AGE_releaseWindow(w) SDL_DestroyWindow(w)

typedef SDL_Window* AGE_Window;
typedef SDL_Renderer* AGE_Renderer;

//init AGE base module
void AGE_init();

//init AGE base module
void AGE_quit();

#endif // AGE_BASE_H
