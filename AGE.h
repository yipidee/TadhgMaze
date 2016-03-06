#ifndef AGE_BASE_H
#define AGE_BASE_H

#include "TextLabel.h"
#include "Queue.h"

#ifdef __linux
    #include <SDL2/SDL.h>
#elif _WIN32
    #include <SDL.h>
#endif

//function macros to hide ugly parts of SDL code
#define AGE_getWindow(w, h) SDL_CreateWindow("Tadhg Maze Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_SHOWN)
#define AGE_getRenderer(window) SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE)
#define AGE_releaseRenderer(r) SDL_DestroyRenderer(r)
#define AGE_releaseWindow(w) SDL_DestroyWindow(w)
#define AGE_CreateTexture(renderer, window, w, h) SDL_CreateTexture(renderer, SDL_GetWindowPixelFormat(window), SDL_TEXTUREACCESS_TARGET, w, h);

//defs to hide SDL code (of no real value, compiled as though SDL)
#define AGE_SetRenderTarget SDL_SetRenderTarget
#define AGE_SetRenderDrawColor SDL_SetRenderDrawColor
#define AGE_RenderClear SDL_RenderClear
#define AGE_RenderDrawLine SDL_RenderDrawLine
#define AGE_RenderCopy SDL_RenderCopy
#define AGE_FreeSurface SDL_FreeSurface
#define AGE_RenderPresent SDL_RenderPresent
#define AGE_RenderDrawRect SDL_RenderDrawRect
#define AGE_CreateTextureFromSurface SDL_CreateTextureFromSurface
#define AGE_getSurfaceFromImage IMG_Load
#define AGE_PollEvent SDL_PollEvent
#define AGE_DestroyTexture SDL_DestroyTexture

typedef SDL_Window* AGE_Window;
typedef SDL_Renderer* AGE_Renderer;
typedef SDL_Texture* AGE_Texture;
typedef SDL_Surface* AGE_Surface;
typedef SDL_Rect AGE_Rect;
typedef SDL_Event AGE_Event;

//init AGE base module
void AGE_init();

//init AGE base module
void AGE_quit();

#endif // AGE_BASE_H
