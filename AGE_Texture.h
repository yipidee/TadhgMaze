#ifndef AGE_TEXTURE_H
#define AGE_TEXTURE_H

#include "AGE_base.h"

typedef struct _AGE_Texture* AGE_Texture;

//returns an AGE_Texture built from image file
AGE_Texture AGE_createTextureFromImage(AGE_Renderer renderer, char* pathToImage);

//create blank AGE_Texture of specified size
AGE_Texture AGE_CreateTexture(AGE_Renderer renderer, AGE_Window window, int w, int h);

//set AGE_Texture to render target
void AGE_SetRenderTarget(AGE_Renderer renderer, AGE_Texture t);

//Render AGE_Texture
void AGE_RenderTexture(AGE_Renderer renderer, AGE_Texture t, AGE_Rect* target, AGE_Rect* dest);

#endif // AGE_TEXTURE_H
