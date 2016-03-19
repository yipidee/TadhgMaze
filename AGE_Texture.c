#include <stdlib.h>
#include "AGE.h"

struct _AGE_Texture
{
    int w, h;
    SDL_Texture* mTexture;
};

//returns an AGE_Texture built from image file
AGE_Texture AGE_createTextureFromImage(AGE_Renderer renderer, char* pathToImage)
{
    AGE_Texture t = malloc(sizeof(struct _AGE_Texture));

    SDL_Surface* tempSurf = IMG_Load(pathToImage);
    t->mTexture = SDL_CreateTextureFromSurface(renderer, tempSurf);
    t->w = tempSurf->w;
    t->h = tempSurf->h;
    SDL_FreeSurface(tempSurf);

    return t;
}

void AGE_DestroyTexture(AGE_Texture t)
{
    SDL_DestroyTexture(t->mTexture);
    free(t);
}

//create blank AGE_Texture of specified size
AGE_Texture AGE_CreateTexture(AGE_Renderer renderer, AGE_Window window, int w, int h)
{
    AGE_Texture t = malloc(sizeof(struct _AGE_Texture));

    t->mTexture = SDL_CreateTexture(renderer, SDL_GetWindowPixelFormat(window), SDL_TEXTUREACCESS_TARGET, w, h);
    t->h = h;
    t->w = w;

    return t;
}

//set AGE_Texture to render target
void AGE_SetRenderTarget(AGE_Renderer renderer, AGE_Texture t)
{
    if(t==NULL) SDL_SetRenderTarget(renderer, NULL);
    else SDL_SetRenderTarget(renderer, t->mTexture);
}

//Render AGE_Texture
void AGE_RenderTexture(AGE_Renderer renderer, AGE_Texture t, AGE_Rect* target, AGE_Rect* dest)
{
    if(t->mTexture != NULL) AGE_RenderCopy(renderer, t->mTexture, target, dest);
}
