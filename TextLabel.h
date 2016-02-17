#ifdef __linux
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_image.h>
    #include <SDL2/SDL_ttf.h>
#elif _Windows
    #include <SDL.h>
    #include <SDL_image.h>
    #include <SDL_ttf.h>
#endif
#include <stdbool.h>

/***********************************************
************************************************
**                                            **
** Colour defs (RGBA)                         **
**                                            **
************************************************
***********************************************/

#define RGB_WHITE {255, 255, 255}
#define RGB_BLACK {0, 0, 0}
#define RGB_RED {255, 0, 0}
#define RGB_GREEN {0, 255, 0}
#define RGB_BLUE {0, 0, 255}
#define RGB_YELLOW {255, 255, 0}
#define RGB_MAGENTA {255, 0, 255}
#define RGB_CYAN {0, 255, 255}

#define MAX_TEXT_LABEL_LENGTH 128

//Data structure for holding TextLabel data
typedef struct TextLabel
{
    char mText[MAX_TEXT_LABEL_LENGTH]; //The actual label text
    int x, y, w, h;             //The position and size of label
    int mCurrLength;            //length of stored text
    TTF_Font* mFont;            //Font used for rendering text
    int mTextSize;              //Text point size
    SDL_Color mFontColour;      //Text colour
    bool _initialised;          //flag to show whether initialised
    SDL_Surface* textSurf;
} TextLabel;

//Creates a label with text at pos x and y
TextLabel TL_createTextLabel(char* text, int x, int y);

//Sets tl's text data
void TL_setText(TextLabel* tl, char* text);

//Set the font to be used for the TextLabel
void TL_setFont(TextLabel* tl, char* pathToFont);

//Get pointer to bounding rect
SDL_Rect TL_getBoundRect(TextLabel* tl);

//Set font size
void TL_setFontSize(TextLabel* tl, int fSize);

//set x
void TL_setX(TextLabel* tl, int x);

//set y
void TL_setY(TextLabel* tl, int y);
