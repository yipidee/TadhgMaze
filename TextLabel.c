#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "TextLabel.h"

#define DEFAULT_TEXT_SIZE 12
#define DEFAULT_FONT "./Yantiq.ttf"

static bool TTF_INITIALIZED = false;

struct _TextLabel
{
    char mText[MAX_TEXT_LABEL_LENGTH]; //The actual label text
    int x, y, w, h;             //The position and size of label
    int mCurrLength;            //length of stored text
    TTF_Font* mFont;            //Font used for rendering text
    int mTextSize;              //Text point size
    SDL_Color mFontColour;      //Text colour
    bool _initialised;          //flag to show whether initialised
    SDL_Surface* textSurf;
};

void TL_setFont(TextLabel tl, char* pathToFont)
{
    TTF_Init();
    if(pathToFont==NULL)
    {
        tl->mFont = TTF_OpenFont(DEFAULT_FONT, tl->mTextSize);
    }else{
        tl->mFont = TTF_OpenFont(pathToFont, tl->mTextSize);
    }
}

void _initTL(TextLabel tl)
{
    tl->mCurrLength = 0;
    SDL_Color c = RGB_YELLOW;
    tl->mFontColour = c;
    tl->mTextSize = DEFAULT_TEXT_SIZE;
    TL_setFont(tl, NULL);
    tl->textSurf = NULL;
    tl->x = 0; tl->y = 0; tl->w = 0; tl->h = 0;
    tl->_initialised = true;
}

void _renderTextToSurface(TextLabel tl)
{
    if(!TTF_INITIALIZED) {TTF_Init(); TTF_INITIALIZED = true;}
    if(tl->textSurf != NULL)
    {
        SDL_FreeSurface(tl->textSurf);
        tl->textSurf = NULL;
    }
    tl->textSurf = TTF_RenderText_Blended(tl->mFont, tl->mText, tl->mFontColour);
    tl->w = tl->textSurf->w; tl->h = tl->textSurf->h;
}

//Get pointer to bounding rect
SDL_Rect TL_getBoundRect(TextLabel tl)
{
    SDL_Rect bb = {tl->x, tl->y, tl->w, tl->h};
    return bb;
}

void TL_renderTextLabel(TextLabel tl, SDL_Renderer* renderer)
{
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, tl->textSurf);
    SDL_Rect bb = TL_getBoundRect(tl);
    SDL_RenderCopy(renderer, textTexture, NULL, &bb);
    SDL_DestroyTexture(textTexture);
}

//Creates a label with text at pos x and y
TextLabel TL_createTextLabel(char* text, int x, int y)
{
    TextLabel tl = malloc(sizeof(struct _TextLabel));
    _initTL(tl);
    int length = strlen(text);
    strcpy(tl->mText, text);
    tl->mCurrLength = length;
    tl->x = x; tl->y = y;
    _renderTextToSurface(tl);
    return tl;
}

void TL_destroyTextLabel(TextLabel tl)
{
    TTF_CloseFont(tl->mFont);
    if(tl->textSurf != NULL) {SDL_FreeSurface(tl->textSurf); tl->textSurf=NULL;}
    free(tl);
}

//Sets tl's text data
void TL_setText(TextLabel tl, char* text)
{
    if(tl->_initialised)
    {
        tl->mCurrLength = strlen(text);
        if(tl->mCurrLength<=MAX_TEXT_LABEL_LENGTH) strcpy(tl->mText, text);
        _renderTextToSurface(tl);
    }
}

void _resizeFont(TextLabel tl)
{
    if(tl->mFont != NULL) TTF_CloseFont(tl->mFont);
    tl->mFont = TTF_OpenFont(DEFAULT_FONT, tl->mTextSize);
}

//Set font size
void TL_setFontSize(TextLabel tl, int fSize)
{
    tl->mTextSize = fSize;
    _resizeFont(tl);
    _renderTextToSurface(tl);
}

//set x
void TL_setX(TextLabel tl, int x)
{
    tl->x = x;
}

//set y
void TL_setY(TextLabel tl, int y)
{
    tl->y = y;
}

//get width of text label
int TL_getWidth(TextLabel tl)
{
    return tl->w;
}

//get Height of text label
int TL_getHeight(TextLabel tl)
{
    return tl->h;
}
